// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquid.h"
#include "RendererInterface.h"
#include "RenderGraphBuilder.h"
#include "RenderingThread.h"
#include "ZibraLiquidBridge.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Renderer/Private/PostProcess/SceneRenderTargets.h"
#include "Engine/Light.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GPUSortManager.h"
#include "ZibraLiquidEmitter.h"
#include "Components/BillboardComponent.h"
#include "ZibraLiquidEnums.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "ZibraLiquidDebug.h"
#include "ZibraLiquidEmitterActor.h"
#include "Engine/VolumeTexture.h"
#include "ZibraNotifications.h"
#include "ZibraLiquidForceField.h"
#include "TextureRenderTargetVolumeResource.h"

#if DX11_SUPPORT
#include "Windows/AllowWindowsPlatformTypes.h"
#include "d3d11.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "ShaderCore.h"
#include "D3D11State.h"
#include "D3D11Resources.h"
#endif //DX11_SUPPORT

#if DX12_SUPPORT
#include "Windows/AllowWindowsPlatformTypes.h"
#include "d3d12.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "ShaderCore.h"
#include "RHICommandList.h"
#include "D3D12RHIPrivate.h"
#include "D3D12Util.h"
#include "D3D12State.h"
#include "D3D12Resources.h"
#include "D3D12RHICommon.h"
#include "D3D12Texture.h"
#include "D3D12Viewport.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12DirectCommandListManager.h"
#endif

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif

DECLARE_GPU_STAT_NAMED(Stat_Zibra_Liquid_Render, TEXT("Zibra Liquid Render"));
DECLARE_GPU_STAT_NAMED(Stat_Zibra_Liquid_SimulatePhysics, TEXT("Zibra Liquid Simulate Physics"));

#if ENGINE_MAJOR_VERSION == 4

void* GetNativePtr(ZibraLiquidProxy::ZibraStructuredBufferRHIRef Resource)
{
#if DX11_SUPPORT
	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			return static_cast<FD3D11StructuredBuffer*>(Resource.GetReference())->Resource;
		}
		else
		{
			return nullptr;
		}
	}
#endif //DX11_SUPPORT
	return nullptr;
}

void* GetNativePtr(ZibraLiquidProxy::ZibraIndexBufferRHIRef Resource)
{
#if DX11_SUPPORT
	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			return static_cast<FD3D11IndexBuffer*>(Resource.GetReference())->Resource;
		}
		else
		{
			return nullptr;
		}
	}
#endif //DX11_SUPPORT
	return nullptr;
}

void* GetNativePtr(ZibraLiquidProxy::ZibraVertexBufferRHIRef Resource)
{
#if DX11_SUPPORT
	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			return static_cast<FD3D11VertexBuffer*>(Resource.GetReference())->Resource;
		}
		else
		{
			return nullptr;
		}
	}
#endif //DX11_SUPPORT
	return nullptr;
}

#elif ENGINE_MAJOR_VERSION == 5

void* GetNativePtr(FBufferRHIRef Resource)
{
	FString RHIName = GDynamicRHI->GetName();

#if DX11_SUPPORT
	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			return static_cast<FD3D11Buffer*>(Resource.GetReference())->Resource;
		}
		else
		{
			checkNoEntry();
			return nullptr;
		}
	}
#endif //DX11_SUPPORT
#if DX12_SUPPORT
	if (RHIName == TEXT("D3D12"))
	{
		if (Resource != nullptr)
		{
			return static_cast<FD3D12Buffer*>(Resource.GetReference())->GetResource()->GetResource();
		}
		else
		{
			checkNoEntry();
			return nullptr;
		}
	}
#endif
	return nullptr;
}

#else
#error Unknown engine version
#endif

FTextureBridge GetTextureBridgePtr(FTexture2DRHIRef Resource)
{
	FTextureBridge tex{};
	FString RHIName = GDynamicRHI->GetName();

#if DX11_SUPPORT
	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			tex.texture = Resource.GetReference()->GetNativeResource();
			tex.format = ToBridgeTextureFormat(Resource.GetReference()->GetFormat());
		}
		else
		{
			checkNoEntry();
			tex.texture = nullptr;
			tex.format = ETextureFormat::ETextureFormat_None;
		}
	}
#endif //DX11_SUPPORT
#if DX12_SUPPORT
	if (RHIName == TEXT("D3D12"))
	{
		if (Resource != nullptr)
		{
			tex.texture = Resource.GetReference()->GetNativeResource();
			tex.format = ToBridgeTextureFormat(Resource.GetReference()->GetFormat());
		}
		else
		{
			checkNoEntry();
			tex.texture = nullptr;
			tex.format = ETextureFormat::ETextureFormat_None;
		}

	}
#endif
	return tex;
}

FTextureBridge GetTextureBridgePtr(FTexture3DRHIRef Resource)
{
	FTextureBridge tex{};
	FString RHIName = GDynamicRHI->GetName();

#if DX11_SUPPORT
	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			FD3D11TextureBase* TextureBasePtr = static_cast<FD3D11TextureBase*>(static_cast<FD3D11Texture3D*>(Resource.GetReference()));
			void* NativePtr = TextureBasePtr->GetResource();
			tex.texture = static_cast<ID3D11Texture3D*>(NativePtr);
			tex.format = ToBridgeTextureFormat(Resource.GetReference()->GetFormat());
		}
		else
		{
			checkNoEntry();
			tex.texture = nullptr;
			tex.format = ETextureFormat::ETextureFormat_None;
		}
	}
#endif //DX11_SUPPORT
#if DX12_SUPPORT
	if (RHIName == TEXT("D3D12"))
	{
		if (Resource != nullptr)
		{
			tex.texture = static_cast<FD3D12Texture3D*>(static_cast<FD3D12Texture3D*>(Resource.GetReference()))->GetResource()->GetResource();
			tex.format = ToBridgeTextureFormat(Resource.GetReference()->GetFormat());
		}
		else
		{
			checkNoEntry();
			tex.texture = nullptr;
			tex.format = ETextureFormat::ETextureFormat_None;
		}
	}
#endif
	return tex;
}

void SetTextureBridgePtr(FTextureBridge* Bridge, FTexture3DRHIRef Resource)
{
#if DX11_SUPPORT
	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		if (Resource != nullptr)
		{
			FD3D11TextureBase* TextureBasePtr = static_cast<FD3D11TextureBase*>(static_cast<FD3D11Texture3D*>(Resource.GetReference()));
			void* NativePtr = TextureBasePtr->GetResource();
			Bridge->texture = static_cast<ID3D11Texture3D*>(NativePtr);
			Bridge->format = ToBridgeTextureFormat(Resource.GetReference()->GetFormat());
		}
		else
		{
			checkNoEntry();
			Bridge->texture = nullptr;
			Bridge->format = ETextureFormat::ETextureFormat_None;
		}
	}
#endif //DX11_SUPPORT
}

int32 AZibraLiquid::ms_NextInstanceId = 0;

TArray<AZibraLiquid*> AZibraLiquid::AllLiquids = TArray<AZibraLiquid*>();

AZibraLiquid::AZibraLiquid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));

	CubeComponent->SetStaticMesh(CubeMesh.Object);
	CubeComponent->SetupAttachment(RootComponent);
	CubeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeComponent->SetSimulatePhysics(false);
	SetActorScale3D(FVector(1, 1, 1));
	CubeComponent->SetRelativeScale3D(FVector(1, 1, 1));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMRMaterial(TEXT("Material'/ZibraAILiquid/ZibraLiquidContent/ZibraMeshRenderMaterial.ZibraMeshRenderMaterial'"));
	if (FoundMRMaterial.Succeeded())
	{
		MeshRenderMaterial = FoundMRMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> FoundMaterialInstance(TEXT("MaterialInstanceConstant'/ZibraAILiquid/ZibraLiquidContent/TransparentFluid.TransparentFluid'"));
	if (FoundMaterialInstance.Succeeded())
	{
		TransparentFluid = FoundMaterialInstance.Object;
		CubeComponent->SetMaterial(0, TransparentFluid);
	}

	const FString RTSDFRender = TEXT("TextureRenderTarget2D'/ZibraAILiquid/ZibraLiquidContent/SDFRender.SDFRender'");

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> FoundSDFRender(*RTSDFRender);

	if (FoundSDFRender.Succeeded())
	{
		SDFRenderRT = FoundSDFRender.Object;
	}
	else
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("SDFRender RenderTarget is not found!"));
	}

#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexture(TEXT("Texture2D'/ZibraAILiquid/ZibraLiquidContent/Icons/icon_ZL.icon_ZL'"));
		SpriteComponent->SetSprite(IconTexture.Object);
		SpriteComponent->SetupAttachment(RootComponent);
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 0));
	}
#endif
	PrimaryActorTick.bCanEverTick = true;

#if ENGINE_MAJOR_VERSION == 4
	// Plugin breaks if resource table caching is enabled
	// Will fix that and remove that override in future updates
	static IConsoleVariable* ResourceTableCachingCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("rhi.ResourceTableCaching"));
	if (ResourceTableCachingCvar != NULL && ResourceTableCachingCvar->GetInt() == 1)
	{
		ResourceTableCachingCvar->Set(0, ECVF_SetByCode);
		return;
	}
#endif
}
void AZibraLiquid::CleanupBeforeBeginPlay()

{
	TArray<AActor*> ActorsToRemove;
	for (AActor* actor : ZibraSDFColliderActors)
	{
		if (actor)
		{
			if (actor->GetWorld() != GetWorld())
			{
				ActorsToRemove.Add(actor);
			}
		}
		else
		{
			ActorsToRemove.Add(actor);
		}
	}
	for (AActor* actor : ActorsToRemove)
	{
		ZibraSDFColliderActors.Remove(actor);
	}

	ActorsToRemove.Empty();

	for (AActor* actor : ManipulatorActors)
	{
		if (actor)
		{
			if (actor->GetWorld() != GetWorld())
			{
				ActorsToRemove.Add(actor);
			}
		}
		else
		{
			ActorsToRemove.Add(actor);
		}
	}

	int EmittersNumber = 0;
	int FFNumber = 0;

	if (ManipulatorActors.Num() > 1)
	{
		for (AActor* actor : ManipulatorActors)
		{
			if (actor)
			{
				if (actor->GetComponentByClass(UZibraLiquidEmitter::StaticClass()))
				{
					EmittersNumber++;
					if (EmittersNumber > 1)
						ActorsToRemove.Add(actor);
				}
				if (actor->GetComponentByClass(UZibraLiquidForceField::StaticClass()))
				{
					FFNumber++;
					if (FFNumber > 1)
						ActorsToRemove.Add(actor);
				}
			}
		}
	}

	for (AActor* actor : ActorsToRemove)
	{
		ManipulatorActors.Remove(actor);
	}

	if (EmittersNumber > 1)
	{
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Only one emitter is allowed in a free version, all emitters but the first will be ignored!"));
		ZibraNotifications::AddNotification(TEXT("Only one emitter is allowed in a free version, all emitters but the first will be ignored!"));
	}

	if (FFNumber > 1)
	{
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Only one force field is allowed in a free version, all emitters but the first will be ignored!"));
		ZibraNotifications::AddNotification(TEXT("Only one force field is allowed in a free version, all emitters but the first will be ignored!"));
	}
	int sdfnumber = 0;
	TArray<AActor*> ActorsToDelete;
	if (ZibraSDFColliderActors.Num() > 5)
	{
		for (AActor* actor : ZibraSDFColliderActors)
		{
			if (actor->GetComponentByClass(UZibraSDFCollider::StaticClass()))
			{
				sdfnumber++;
				if (sdfnumber > 5)
				{
					ActorsToDelete.Add(actor);
				}
			}
		}
		if (sdfnumber >= 5)
		{
			UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Maximum number of SDF colldiers is 5 in free version, removing all colliders with id >= 5"));
			ZibraNotifications::AddNotification(TEXT("Maximum number of SDF colldiers is 5 in free version, removing all colliders with id >= 5"));

			for (AActor* actor : ActorsToDelete)
			{
				ZibraSDFColliderActors.Remove(actor);
			}
		}
	}
}
void AZibraLiquid::BeginPlay()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ZibraLiquid_BeginPlay);

	Super::BeginPlay();

	CleanupBeforeBeginPlay();
	
	Proxy = new ZibraLiquidProxy{};

	// We don't currently support running with RHI thread enabled
	// Will add support and remove that call in future updates
	SetRHIThreadEnabled(false, false);

	manipulators.Empty();
	ZibraSDFColliders.Empty();



	FString RHIName = GDynamicRHI->GetName();
#if ENGINE_MAJOR_VERSION == 4
	if (RHIName != TEXT("D3D11"))
	{
		ZibraNotifications::AddNotification("Only DirectX 11 is currently supported. Please select DirectX 11 as Default RHI in Project settings");
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Only DirectX 11 is currently supported. Please select DirectX 11 as Default RHI in Project settings"));
		bInitialized = false;
		return;
	}
#elif ENGINE_MAJOR_VERSION == 5
	if (RHIName != TEXT("D3D11") && RHIName != TEXT("D3D12"))
	{
		ZibraNotifications::AddNotification("Only DirectX 11 and 12 is currently supported. Please select DirectX 11 as Default RHI in Project settings");
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Only DirectX 11 and 12 is currently supported. Please select DirectX 11 as Default RHI in Project settings"));
		bInitialized = false;
		return;
	}
#else
#error Unknown engine version
#endif

	if (!CheckDll())
	{
		ZibraNotifications::AddNotification("Could not find the Zibra DLL!");
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Could not find the Zibra DLL!"));
		bInitialized = false;
		return;
	}

	CubeComponent->SetVisibility(true);
	if (!TransparentFluid)
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("TransparentFluid is not found!"));

	if (!CubeComponent)
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("CubeComponent is not found!"));

	FluidMaterial = UMaterialInstanceDynamic::Create(MeshRenderMaterial, CubeComponent);

	if (!FluidMaterial)
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("FluidMaterial is not found!"));

	CubeComponent->SetMaterial(0, FluidMaterial);

	if (bInitialized)
	{
		return;
	}

	ContainerPosition = GetActorLocation();
	ContainerSizeScaled = ContainerSize * 100;

	Proxy->InstanceID = ms_NextInstanceId++;

	InitializeParameters();
	CreateBuffers();
	InitializeManipulators();
	UpdateRegisterParticlesBuffersParams();
	UpdateRegisterSolverBuffersBridgeParams();

	uint32 manipSize = (uint32)ManipulatorManager->Elements * STATISTICS_PER_MANIPULATOR * sizeof(int32);
	InitializeGPUReadbackParams.ReadbackBufferSize = sizeof(int32) + manipSize;
	InitializeGPUReadbackParams.MaxInFlightFrames = maxFramesInFlight;

	FRegisterSolverBuffersBridgeParams RegisterSolverBuffersBridgeParamsCopy = RegisterSolverBuffersBridgeParams;
	FRegisterParticlesBuffersBridgeParams RegisterParticlesBuffersParamsCopy = RegisterParticlesBuffersParams;
	FRegisterManipulatorsBridgeParams RegisterManipulatorsBridgeParamsCopy = RegisterManipulatorsBridgeParams;
	FInitializeGPUReadbackParams InitializeGPUReadbackParamsCopy = InitializeGPUReadbackParams;

	FInitRHIResourcesData InitRHIResourcesData;
	InitRHIResourcesData.DensityTextureBridge = &Proxy->DensityTextureBridge;
	InitRHIResourcesData.GridNormalTextureBridge = &Proxy->GridNormalTextureBridge;
	InitRHIResourcesData.RenderTargetGridDensity = RenderTargetGridDensity;
	InitRHIResourcesData.RenderTargetGridNormals = RenderTargetGridNormals;

	ZibraLiquidProxy* ProxyCopy = Proxy;
	ENQUEUE_RENDER_COMMAND(Init)(
		[ProxyCopy, RegisterSolverBuffersBridgeParamsCopy, RegisterParticlesBuffersParamsCopy, RegisterManipulatorsBridgeParamsCopy, InitializeGPUReadbackParamsCopy, InitRHIResourcesData](FRHICommandListImmediate& RHICmdList)
		{
			FRegisterSolverBuffersBridgeParams RegisterSolverBuffersBridgeParams = RegisterSolverBuffersBridgeParamsCopy;
			AZibraLiquid::InitRHIResources(InitRHIResourcesData, &RegisterSolverBuffersBridgeParams);
			Init(ProxyCopy, RegisterSolverBuffersBridgeParams, RegisterParticlesBuffersParamsCopy, RegisterManipulatorsBridgeParamsCopy, InitializeGPUReadbackParamsCopy);
		});
	FlushRenderingCommands();

	if (bHaveEmitter)
		bInitialized = true;

	RegisterRenderThreadCallback();

	AllLiquids.Add(this);
	
	bForceTextureUpdate = true;
}

void AZibraLiquid::Tick(float inDeltaTime)
{
	Super::Tick(inDeltaTime);

#if WITH_EDITOR
	SpriteComponent->SetRelativeLocation(FVector(0, 0, 0));
#endif

	ContainerSizeScaled = ContainerSize * 100;

#if WITH_EDITOR
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		EditorTick(inDeltaTime);
	}
	else
#endif
	{
		if (!bInitialized)
			return;


#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 27)
	
		const int32 Width = FMath::Max((int32)CameraRenderParams.CameraResolution.X, 1);
		const int32 Height = FMath::Max((int32)CameraRenderParams.CameraResolution.Y, 1);

		if (OldWidthGameThread != Width || OldHeightGameThread != Height)
		{
			bIsDirtyGameThread = true;
		}

		OldWidthGameThread = Width > 1 ? Width : 0;
		OldHeightGameThread = Height > 1 ? Height : 0;

		if (bIsDirtyGameThread || b2DTexturesDirty)
		{
			{
				Color0UT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_A32B32G32R32F, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable | TexCreate_Shared, TEXT("Color0UT"));
				Color1UT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_A32B32G32R32F, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable | TexCreate_Shared);
				DepthUT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_DepthStencil, TexCreate_ShaderResource | TexCreate_DepthStencilTargetable, TEXT("DepthUT"));
			}

			FlushRenderingCommands();
			Proxy->Color0Bridge = GetTextureBridgePtr(Color0UT->GetResource()->GetTexture2DRHI());
			Proxy->Color1Bridge = GetTextureBridgePtr(Color1UT->GetResource()->GetTexture2DRHI());
			Proxy->DepthBridge = GetTextureBridgePtr(DepthUT->GetResource()->GetTexture2DRHI());
			b2DTexturesDirty = false;
			bIsDirtyGameThread = false;
			bNeedRegister2dTextures = true;
		}
#endif
		AllZibraSDFColliders = UZibraSDFCollider::AllColliders;
		AllManipulators = UZibraManipulator::AllManipulators;

		ZibraLiquidProxy* ProxyCopy = Proxy;
		int32 ManipulatorManagerElements = ManipulatorManager->Elements;
		int IterationsPerFrameCopy = IterationsPerFrame;
		Timestep = FMath::Min(SimTimePerSec * inDeltaTime / (float)IterationsPerFrame, TimeStepMax);

		SetFluidParameters();
		ManipulatorManager->UpdateDynamic(this, Timestep);
		TArray<FManipulatorParams> ManipulatorParamsCopy = ManipulatorManager->ManipulatorParams;
		if (bInitialized && bRunSimulation)
		{
			FSimulationParams FluidParametersCopy = FluidParameters;
			ENQUEUE_RENDER_COMMAND(SimulationCallBack)(
				[ProxyCopy, IterationsPerFrameCopy, FluidParametersCopy, ManipulatorParamsCopy, ManipulatorManagerElements](FRHICommandListImmediate& RHICmdList)
				{
					SimulationCallBack(ProxyCopy, RHICmdList, IterationsPerFrameCopy, FluidParametersCopy, ManipulatorParamsCopy, ManipulatorManagerElements);
				});

			// Update internal time
			SimulationInternalTime += Timestep;
			SimulationInternalFrame++;
		}

		UpdateReadback();

		ParticleDiameter = MaterialParameters.ParticleScale * CellSize /
			(float)FMath::Pow(SolverParameters.ParticleDensity, 0.333f);
		ParticleMass = 1.0f;

		CubeComponent->GetBodyInstance()->AddCustomPhysics(CalculateCustomPhysics);
	}

	UpdateMaterial();
}

void AZibraLiquid::BeginDestroy()
{
	Super::BeginDestroy();

	AllLiquids.Remove(this);

	if (bInitialized)
	{
		bInitialized = false;
		bIsDeleting = true;

		ZibraLiquidBridge::ReleaseResources(Proxy->InstanceID);

		FluidMaterial = nullptr;
		ManipulatorManager = nullptr;
	}
}


bool AZibraLiquid::IsReadyForFinishDestroy()
{
	if (!Super::IsReadyForFinishDestroy())
	{
		return false;
	}

	if (bIsDeleting)
	{
		return ZibraLiquidBridge::GarbageCollect() != 0;
	}

	return true;
}

void AZibraLiquid::FinishDestroy()
{
	Super::FinishDestroy();

	if (bIsDeleting)
	{
		ReleaseGPUResources();
	}

	delete Proxy;
}

void AZibraLiquid::InitializeParameters()
{
	bIsEnabled = true;

	NumParticles = MaxNumParticles;

	CellSize = float(FMath::Max3(ContainerSizeScaled.X, ContainerSizeScaled.Y, ContainerSizeScaled.Z)) / float(GridResolution);

	GridSize = FIntVector(
		FMath::CeilToInt(ContainerSizeScaled.X / CellSize),
		FMath::CeilToInt(ContainerSizeScaled.Y / CellSize),
		FMath::CeilToInt(ContainerSizeScaled.Z / CellSize));

	if (GridSize.X != GridResolution && GridSize.Y != GridResolution && GridSize.Z != GridResolution)
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("GridSize is wrong! At least one dimesnion must be equal to GridResolution!"));
	}
	if (GridSize.X > GridResolution || GridSize.Y > GridResolution || GridSize.Z > GridResolution)
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("GridSize is wrong! At least one dimesnion is greater than GridResolution!"));
	}
	if (GridSize.X == 0 || GridSize.Y == 0 || GridSize.Z == 0)
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("GridSize is wrong! At least one dimesnion is equal to 0!"));
	}

	NumNodes = GridSize[0] * GridSize[1] * GridSize[2];
	SimulationInternalTime = 0.0f;
	SimulationInternalFrame = 0;
	NumParticlesRounded = (int)FMath::CeilToInt((float)MaxNumParticles / MPM_THREADS) * MPM_THREADS; // round to workgroup size

	SetFluidParameters();
}

void AZibraLiquid::InitializeManipulators()
{
	for (AActor* actor : ManipulatorActors)
	{
		if (actor)
		{
			TArray<UZibraManipulator*> Components;
			actor->GetComponents<UZibraManipulator>(Components);
			if (Components.Num() > 0)
			{
				for (int32 i = 0; i < Components.Num(); i++)
					if (!manipulators.Contains(Components[i]))
					{
						manipulators.Add(Components[i]);
					}
			}
			else
			{
				UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Actor %s has no Manipulator Component!"), *actor->GetName());
			}
		}
		else
		{
			UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Empty element in Manipulators list!"));
		}
	}

	for (AActor* actor : ZibraSDFColliderActors)
	{
		if (actor)
		{
			TArray<UZibraSDFCollider*> Components;
			actor->GetComponents<UZibraSDFCollider>(Components);
			if (Components.Num() > 0)
			{
				for (int32 i = 0; i < Components.Num(); i++)
					if (!ZibraSDFColliders.Contains(Components[i]))
					{
						ZibraSDFColliders.Add(Components[i]);
					}
			}
			else
			{
				UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Actor %s has no SDF Collider Component!"), *actor->GetName());
			}
		}
		else
		{
			UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Empty element in SDF Colliders list!"));
		}
	}

	bHaveEmitter = false;
	for (auto manipulator : manipulators)
	{
		if (manipulator && manipulator->IsA(UZibraLiquidEmitter::StaticClass()))
		{
			bHaveEmitter = true;
			break;
		}
	}
	if (!bHaveEmitter)
	{
		ZibraNotifications::AddNotification("Liquid creation failed. Liquid have neither initial state nor emitters!");
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Liquid creation failed. Liquid have neither initial state nor emitters!"));
		bInitialized = false;
	}
	ManipulatorManager = NewObject<UZibraManipulatorManager>();
	if (ManipulatorManager)
	{
		ManipulatorManager->UpdateConst(manipulators, ZibraSDFColliders);
		ManipulatorManager->UpdateDynamic(this);

		
		{
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("EmbeddingsTextureRHI"));
			Proxy->EmbeddingsTextureRHI = RHICreateTexture3D(1, 1, 1, (int)EPixelFormat::PF_A32B32G32R32F, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
			Proxy->EmbeddingsTextureBridge = GetTextureBridgePtr(Proxy->EmbeddingsTextureRHI);
		}
		{
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("SDFGridTextureRHI"));
			Proxy->SDFGridTextureRHI = RHICreateTexture3D(1, 1, 1, (int)EPixelFormat::PF_R16F, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
			Proxy->SDFGridTextureBridge = GetTextureBridgePtr(Proxy->SDFGridTextureRHI);
		}
		

		const int32 ConstDataLength = ManipulatorManager->ConstAdditionalData.Num();

		{
			int32 ElementSize = sizeof(FManipulatorParams);
			int32 ElementsCount = FMath::Max(ManipulatorManager->Elements, 1);
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("DynamicManipulatorData"));
			Proxy->DynamicManipulatorDataRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * ElementsCount, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}
		{
			int32 ElementSize = sizeof(int);
			int32 ElementsCount = FMath::Max(ConstDataLength, 1);
			int constDataSize = ElementSize * ElementsCount;
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("ConstManipulatorData"));
			Proxy->ConstManipulatorDataRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * ElementsCount, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}

		{
			int32 ElementSize = sizeof(int);
			int32 ElementsCount = FMath::Max(1, ManipulatorManager->Elements * STATISTICS_PER_MANIPULATOR);
			TResourceArray<int> InitData;
			InitData.Init(0, ElementsCount);
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("ManipulatorStatisticsRHI"), &InitData);
			Proxy->ManipulatorStatisticsRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * ElementsCount, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}

		RegisterManipulatorsBridgeParams.ManipulatorNum = ManipulatorManager->Elements;
		RegisterManipulatorsBridgeParams.ManipulatorBufferDynamic = GetNativePtr(Proxy->DynamicManipulatorDataRHI);
		RegisterManipulatorsBridgeParams.ManipulatorBufferConst = GetNativePtr(Proxy->ConstManipulatorDataRHI);
		RegisterManipulatorsBridgeParams.ManipulatorBufferStatistics = GetNativePtr(Proxy->ManipulatorStatisticsRHI);
		RegisterManipulatorsBridgeParams.ManipulatorParams = ManipulatorManager->ManipulatorParams.GetData();
		RegisterManipulatorsBridgeParams.ConstDataSize = ConstDataLength;
		RegisterManipulatorsBridgeParams.ConstManipulatorData = ManipulatorManager->ConstAdditionalData.GetData();
		RegisterManipulatorsBridgeParams.ManipIndices = &ManipulatorManager->ManipulatorIndices;
		RegisterManipulatorsBridgeParams.EmbeddingsTexture = Proxy->EmbeddingsTextureBridge;
		RegisterManipulatorsBridgeParams.SDFGridTexture = Proxy->SDFGridTextureBridge;

		if (ManipulatorManager->Embeddings.Num() > 0)
		{
			RegisterManipulatorsBridgeParams.EmbeddigsData.dataSize = sizeof(uint8) * ManipulatorManager->Embeddings.Num();
			RegisterManipulatorsBridgeParams.EmbeddigsData.data = ManipulatorManager->Embeddings.GetData();
			RegisterManipulatorsBridgeParams.EmbeddigsData.rowPitch = 4 * sizeof(uint8) * Proxy->EmbeddingsTextureRHI.GetReference()->GetTexture3D()->GetSizeX();
			RegisterManipulatorsBridgeParams.EmbeddigsData.dimensionX = Proxy->EmbeddingsTextureRHI.GetReference()->GetTexture3D()->GetSizeX();
			RegisterManipulatorsBridgeParams.EmbeddigsData.dimensionY = Proxy->EmbeddingsTextureRHI.GetReference()->GetTexture3D()->GetSizeY();
			RegisterManipulatorsBridgeParams.EmbeddigsData.dimensionZ = Proxy->EmbeddingsTextureRHI.GetReference()->GetTexture3D()->GetSizeZ();
		}

		if (ManipulatorManager->SDFGrid.Num() > 0)
		{
			RegisterManipulatorsBridgeParams.SDFGridData.dataSize = sizeof(uint8) * ManipulatorManager->SDFGrid.Num();
			RegisterManipulatorsBridgeParams.SDFGridData.data = ManipulatorManager->SDFGrid.GetData();
			RegisterManipulatorsBridgeParams.SDFGridData.rowPitch = sizeof(uint8) * 2 * Proxy->SDFGridTextureRHI.GetReference()->GetTexture3D()->GetSizeX();
			RegisterManipulatorsBridgeParams.SDFGridData.dimensionX = Proxy->SDFGridTextureRHI.GetReference()->GetTexture3D()->GetSizeX();
			RegisterManipulatorsBridgeParams.SDFGridData.dimensionY = Proxy->SDFGridTextureRHI.GetReference()->GetTexture3D()->GetSizeY();
			RegisterManipulatorsBridgeParams.SDFGridData.dimensionZ = Proxy->SDFGridTextureRHI.GetReference()->GetTexture3D()->GetSizeZ();
		}
	}
	else
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("No ManipulatorManager has been set"));
	}
}

FRHIResourceCreateInfo AZibraLiquid::GetCreateInfo(TCHAR* DebugName, FResourceArrayInterface* Data)
{
#if ENGINE_MAJOR_VERSION == 4
	FRHIResourceCreateInfo CreateInfo(Data);
	CreateInfo.DebugName = DebugName;
#elif ENGINE_MAJOR_VERSION == 5
	FRHIResourceCreateInfo CreateInfo(DebugName, Data);
#endif
	return CreateInfo;
}

FRHIResourceCreateInfo AZibraLiquid::GetCreateInfo(TCHAR* DebugName)
{
#if ENGINE_MAJOR_VERSION == 4
	FRHIResourceCreateInfo CreateInfo;
	CreateInfo.DebugName = DebugName;
#elif ENGINE_MAJOR_VERSION == 5
	FRHIResourceCreateInfo CreateInfo(*DebugName);
#endif
	return CreateInfo;
}

void AZibraLiquid::CreateBuffers()
{
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridDataRHI"));
		Proxy->GridDataRHI = RHICreateStructuredBuffer(ElementSize, 4 * ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridNormalRHI"));
		Proxy->GridNormalRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridBlur0RHI"));
		Proxy->GridBlur0RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridBlur1RHI"));
		Proxy->GridBlur1RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridSDFRHI"));
		Proxy->GridSDFRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 2 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("IndexGridRHI"));
		Proxy->IndexGridRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("PositionMassCopyRHI"));
		Proxy->PositionMassCopyRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumParticlesRounded, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("NodeParticlePairs0RHI"));
		Proxy->NodeParticlePairs0RHI = RHICreateStructuredBuffer(ElementSize, 2 * ElementSize * NumParticlesRounded, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("NodeParticlePairs1RHI"));
		Proxy->NodeParticlePairs1RHI = RHICreateStructuredBuffer(ElementSize, 2 * ElementSize * NumParticlesRounded, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int RadixWorkGroups1 = FMath::CeilToInt((float)MaxNumParticles / (float)(2 * RADIX_THREADS));

		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("RadixGroupData1RHI"));
		Proxy->RadixGroupData1RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * RadixWorkGroups1 * HISTO_WIDTH, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int RadixWorkGroups2 = FMath::CeilToInt((float)MaxNumParticles / (float)(RADIX_THREADS * RADIX_THREADS));

		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("RadixGroupData2RHI"));
		Proxy->RadixGroupData2RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * RadixWorkGroups2 * HISTO_WIDTH, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int RadixWorkGroups2 = FMath::CeilToInt((float)MaxNumParticles / (float)(RADIX_THREADS * RADIX_THREADS));
		int RadixWorkGroups3 = FMath::CeilToInt((float)RadixWorkGroups2 / (float)RADIX_THREADS);

		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("RadixGroupData3RHI"));
		Proxy->RadixGroupData3RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * (RadixWorkGroups3 + 1) * HISTO_WIDTH, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		TResourceArray<int> ParticleNumber;
		ParticleNumber.Init(0, 128);
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("ParticleNumberRHI"), &ParticleNumber);
		Proxy->ParticleNumberRHI = RHICreateStructuredBuffer(ElementSize, ParticleNumber.GetResourceDataSize(), BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("PositionMassRHI"));
		Proxy->PositionMassRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * MaxNumParticles, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(float);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("PositionRadiusRHI"));
		Proxy->PositionRadiusRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * MaxNumParticles, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 2 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("Affine0RHI"));
		Proxy->Affine0RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * 4 * NumParticlesRounded, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 2 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("Affine1RHI"));
		Proxy->Affine1RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * 4 * NumParticlesRounded, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("CountersRHI"));
		Proxy->CountersRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * 8, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	/*{
		int32 ElementSize = sizeof(int);
		TResourceArray<int> initialDataArray;
		int initialData[8] = { 0, 1, 0, 0, 1, 1, 1, 1 };
		for (int value : initialData)
		{
			initialDataArray.Add(value);
		}
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("IndirectArgumentsRHI"), &initialDataArray);
#if ENGINE_MAJOR_VERSION == 4
		IndirectArgumentsRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * 8, BUF_UnorderedAccess | BUF_ShaderResource | BUF_DrawIndirect, CreateInfo);
#elif ENGINE_MAJOR_VERSION == 5
		IndirectArgumentsRHI = RHICreateBuffer(ElementSize * 8, BUF_UnorderedAccess | BUF_ShaderResource | BUF_DrawIndirect, 0, ERHIAccess::Unknown, CreateInfo);
#endif
	}*/
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("VertexIDGridRHI"));
		Proxy->VertexIDGridRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("VertexBuffer0RHI"));
		Proxy->VertexBuffer0RHI = RHICreateVertexBuffer(ElementSize * NumNodes, BUF_Shared | BUF_UnorderedAccess | BUF_VertexBuffer | BUF_ShaderResource, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("VertexBuffer1RHI"));
		Proxy->VertexBuffer1RHI = RHICreateVertexBuffer(ElementSize * NumNodes, BUF_Shared | BUF_UnorderedAccess | BUF_VertexBuffer | BUF_ShaderResource, CreateInfo);
	}
	{
		int32 ElementSize = 4 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("VertexPropertiesRHI"));
		Proxy->VertexPropertiesRHI = RHICreateVertexBuffer(ElementSize * NumNodes, BUF_Shared | BUF_UnorderedAccess | BUF_VertexBuffer | BUF_ShaderResource, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("MeshRenderIndexBufferRHI"));
		Proxy->MeshRenderIndexBufferRHI = RHICreateIndexBuffer(ElementSize, 3 * ElementSize * NumNodes, BUF_Shared | BUF_UnorderedAccess | BUF_IndexBuffer | BUF_ShaderResource, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("TransferDataBufferRHI"));
		Proxy->TransferDataBufferRHI = RHICreateStructuredBuffer(ElementSize, ElementSize, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("QuadBufferRHI"));
		Proxy->QuadBufferRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{
		int32 ElementSize = 2 * sizeof(int);
		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("IndexGridRHI"));
		Proxy->IndexGridRHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumNodes, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
	}
	{	
		RenderTargetGridNormals = NewObject<UTextureRenderTargetVolume>(GetWorld(), TEXT("RenderTargetGridNormals"));
		RenderTargetGridNormals->Init(GridSize.X, GridSize.Y, GridSize.Z, EPixelFormat::PF_FloatRGBA);
		RenderTargetGridNormals->bCanCreateUAV = true;

		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("GridNormalTextureRHI"));
		Proxy->GridNormalTextureRHI = RHICreateTexture3D(GridSize.X, GridSize.Y, GridSize.Z, PF_FloatRGBA, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
#if ENGINE_MAJOR_VERSION == 5
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(RenderTargetGridNormals->GetResource());
#else
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(RenderTargetGridNormals->Resource);
#endif
		InResource->SetTextureReference(Proxy->GridNormalTextureRHI.GetReference()->GetTextureReference());
	}
	{
		RenderTargetGridDensity = NewObject<UTextureRenderTargetVolume>(GetWorld(), TEXT("RenderTargetGridDensity"));
		RenderTargetGridDensity->Init(GridSize.X, GridSize.Y, GridSize.Z, EPixelFormat::PF_R32_FLOAT);
		RenderTargetGridDensity->bCanCreateUAV = true;

		FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("DensityTextureRHI"));
		Proxy->DensityTextureRHI = RHICreateTexture3D(GridSize.X, GridSize.Y, GridSize.Z, PF_R32_FLOAT, 1, TexCreate_ShaderResource | TexCreate_UAV, CreateInfo);
#if ENGINE_MAJOR_VERSION == 5
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(RenderTargetGridDensity->GetResource());
#else
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(RenderTargetGridDensity->Resource);
#endif

		InResource->SetTextureReference(Proxy->DensityTextureRHI.GetReference()->GetTextureReference());
	}
}

void AZibraLiquid::ReleaseGPUResources()
{
	Proxy->PositionMassRHI.SafeRelease();
	Proxy->PositionRadiusRHI.SafeRelease();
	Proxy->Affine0RHI.SafeRelease();
	Proxy->Affine1RHI.SafeRelease();
	Proxy->GridDataRHI.SafeRelease();
	Proxy->IndexGridRHI.SafeRelease();
	Proxy->NodeParticlePairs0RHI.SafeRelease();
	Proxy->NodeParticlePairs1RHI.SafeRelease();

	Proxy->PositionMassCopyRHI.SafeRelease();
	Proxy->GridNormalRHI.SafeRelease();
	Proxy->GridBlur0RHI.SafeRelease();
	Proxy->GridBlur1RHI.SafeRelease();
	Proxy->GridSDFRHI.SafeRelease();
	Proxy->RadixGroupData1RHI.SafeRelease();
	Proxy->RadixGroupData2RHI.SafeRelease();
	Proxy->RadixGroupData3RHI.SafeRelease();

	Proxy->ParticleNumberRHI.SafeRelease();
	Proxy->DynamicManipulatorDataRHI.SafeRelease();
	Proxy->ConstManipulatorDataRHI.SafeRelease();
	Proxy->ManipulatorStatisticsRHI.SafeRelease();
	Proxy->CountersRHI.SafeRelease();

	Proxy->VertexIDGridRHI.SafeRelease();
	Proxy->VertexBuffer0RHI.SafeRelease();
	Proxy->VertexBuffer1RHI.SafeRelease();
	Proxy->TransferDataBufferRHI.SafeRelease();
	Proxy->MeshRenderIndexBufferRHI.SafeRelease();
	Proxy->QuadBufferRHI.SafeRelease();
	Proxy->VertexPropertiesRHI.SafeRelease();

	Proxy->GridNormalTextureRHI.SafeRelease();
	Proxy->DensityTextureRHI.SafeRelease();

	Proxy->AtomicGridRHI.SafeRelease();
	Proxy->JFAGrid0RHI.SafeRelease();
	Proxy->JFAGrid1RHI.SafeRelease();
	Proxy->SurfaceGridTypeRHI.SafeRelease();

	Proxy->SDFGridTextureRHI.SafeRelease();
	Proxy->EmbeddingsTextureRHI.SafeRelease();

	b3DTexturesDirty = true;
	bIsDirty = true;
}

void AZibraLiquid::InitRHIResources(FInitRHIResourcesData data, FRegisterSolverBuffersBridgeParams* RegisterSolverBuffersBridgeParams)
{
	//InitDynamicRHI must be performed in rendering thread
	check(IsInRenderingThread());

	{	
#if	ENGINE_MAJOR_VERSION == 5
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(data.RenderTargetGridDensity->GetResource());
#else
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(data.RenderTargetGridDensity->Resource);
#endif
		InResource->InitDynamicRHI();
		*data.DensityTextureBridge = GetTextureBridgePtr(InResource->GetTextureRHI());
		RegisterSolverBuffersBridgeParams->GridDensity = *data.DensityTextureBridge;	
	}
	{
#if	ENGINE_MAJOR_VERSION == 5
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(data.RenderTargetGridNormals->GetResource());
#else
		FTextureRenderTargetVolumeResource* InResource = static_cast<FTextureRenderTargetVolumeResource*>(data.RenderTargetGridNormals->Resource);
#endif
		InResource->InitDynamicRHI();
		*data.GridNormalTextureBridge = GetTextureBridgePtr(InResource->GetTextureRHI());
		RegisterSolverBuffersBridgeParams->GridNormals = *data.GridNormalTextureBridge;
	}	
}

void AZibraLiquid::SetFluidParameters()
{
	ValidateGravity();

	ContainerPosition = GetActorLocation();
#if ENGINE_MAJOR_VERSION == 4
	FluidParameters.GridSize = FVector(GridSize);
	FluidParameters.ContainerScale = ContainerSizeScaled;
	FluidParameters.ContainerPos = ContainerPosition;
	FluidParameters.Gravity = SolverParameters.Gravity / 100.0f;
#elif ENGINE_MAJOR_VERSION == 5
	FluidParameters.GridSize = FVector3f(GridSize);
	FluidParameters.ContainerScale = (FVector3f)ContainerSizeScaled;
	FluidParameters.ContainerPos = (FVector3f)ContainerPosition;
	FluidParameters.Gravity = (FVector3f)(SolverParameters.Gravity / 100.0f);
#endif

	FluidParameters.TimeStep = Timestep;
	FluidParameters.NodeCount = NumNodes;
	FluidParameters.SimulationFrame = SimulationInternalFrame;
	FluidParameters.DensityBlurRadius = MaterialParameters.FluidSurfaceBlur;
	FluidParameters.LiquidIsosurfaceThreshold = RenderingParameters.IsoSurfaceLevel;
	FluidParameters.VertexOptimizationStep = RenderingParameters.VertexOptimizationStep;
	FluidParameters.AffineAmount = 4.0f * (1.0f - SolverParameters.Viscosity);
	FluidParameters.VelocityLimit = SolverParameters.MaximumVelocity;
	FluidParameters.LiquidStiffness = SolverParameters.FluidStiffness;
	FluidParameters.RestDensity = SolverParameters.ParticleDensity;
	FluidParameters.AffineDivergenceDecay = 1; DivergenceDecayCurve(Timestep);
	FluidParameters.MaxParticleCount = MaxNumParticles;
	FluidParameters.VisualizeSDF = bVisualizeSceneSDF ? 1 : 0;
}

FColor AZibraLiquid::GetColor()
{
	return MaterialParameters.Color;
}

void AZibraLiquid::SetColor(FColor NewValue)
{
	MaterialParameters.Color = NewValue;
}

FColor AZibraLiquid::GetEmissiveColor()
{
	return MaterialParameters.EmissiveColor;
}

void AZibraLiquid::SetEmissiveColor(FColor NewValue)
{
	MaterialParameters.EmissiveColor = NewValue;
}

float AZibraLiquid::GetRoughness()
{
	return MaterialParameters.Roughness;
}

void AZibraLiquid::SetRoughness(float NewValue)
{
	MaterialParameters.Roughness = NewValue;
}

float AZibraLiquid::GetMetalness()
{
	return MaterialParameters.Metalness;
}

void AZibraLiquid::SetMetalness(float NewValue)
{
	MaterialParameters.Metalness = NewValue;
}

float AZibraLiquid::GetScatteringAmount()
{
	return MaterialParameters.ScatteringAmount;
}

void AZibraLiquid::SetScatteringAmount(float NewValue)
{
	MaterialParameters.ScatteringAmount = NewValue;
}

float AZibraLiquid::GetOpacity()
{
	return MaterialParameters.Opacity;
}

void AZibraLiquid::SetOpacity(float NewValue)
{
	MaterialParameters.Opacity = NewValue;
}

float AZibraLiquid::GetAbsorptionAmount()
{
	return MaterialParameters.AbsorptionAmount;
}

void AZibraLiquid::SetAbsorptionAmount(float NewValue)
{
	MaterialParameters.AbsorptionAmount = NewValue;
}

float AZibraLiquid::GetIndexOfRefraction()
{
	return MaterialParameters.IndexOfRefraction;
}

void AZibraLiquid::SetIndexOfRefraction(float NewValue)
{
	MaterialParameters.IndexOfRefraction = NewValue;
}

float AZibraLiquid::GetFluidSurfaceBlur()
{
	return MaterialParameters.FluidSurfaceBlur;
}

void AZibraLiquid::SetFluidSurfaceBlur(float NewValue)
{
	MaterialParameters.FluidSurfaceBlur = NewValue;
}

float AZibraLiquid::GetParticleScale()
{
	return MaterialParameters.ParticleScale;
}

void AZibraLiquid::SetParticleScale(float NewValue)
{
	MaterialParameters.ParticleScale = NewValue;
}

float AZibraLiquid::GetBlurRadius()
{
	return MaterialParameters.BlurRadius;
}

void AZibraLiquid::SetBlurRadius(float NewValue)
{
	MaterialParameters.BlurRadius = NewValue;
}

void AZibraLiquid::SetRayMarchingResolutionDownscale(float NewValue)
{
	RenderingParameters.RayMarchingResolutionDownscale = NewValue;
}

float AZibraLiquid::GetRayMarchingResolutionDownscale()
{
	return RenderingParameters.RayMarchingResolutionDownscale;
}

ELiquidRefractionQuality AZibraLiquid::GetRefractionQuality()
{
	return RenderingParameters.RefractionQuality;
}

void AZibraLiquid::SetRefractionQuality(ELiquidRefractionQuality NewValue)
{
	RenderingParameters.RefractionQuality = NewValue;
}

ERayMarchingBounces AZibraLiquid::GetRefractionBounces()
{
	return RenderingParameters.RefractionBounces;
}

void AZibraLiquid::SetRefractionBounces(ERayMarchingBounces NewValue)
{
	RenderingParameters.RefractionBounces = NewValue;
}

bool AZibraLiquid::UnderwaterRenderEnabled()
{
	return RenderingParameters.bUnderwaterRender;
}

void AZibraLiquid::SetUnderwaterRender(bool NewValue)
{
	RenderingParameters.bUnderwaterRender = NewValue;
}

float AZibraLiquid::GetMaxLiquidMeshSize()
{
	return RenderingParameters.MaxLiquidMeshSize;
}

void AZibraLiquid::SetMaxLiquidMeshSize(float NewValue)
{
	RenderingParameters.MaxLiquidMeshSize = NewValue;
}

int AZibraLiquid::GetAdditionalJFAIterations()
{
	return RenderingParameters.AdditionalJFAIterations;
}

void AZibraLiquid::SetAdditionalJFAIterations(int NewValue)
{
	RenderingParameters.AdditionalJFAIterations = NewValue;
}

int32 AZibraLiquid::GetVertexOptimizationIterations()
{
	return RenderingParameters.VertexOptimizationIterations;
}

void AZibraLiquid::SetVertexOptimizationIterations(int32 NewValue)
{
	RenderingParameters.VertexOptimizationIterations = NewValue;
}

int AZibraLiquid::GetMeshOptimizationIterations()
{
	return RenderingParameters.MeshOptimizationIterations;
}

void AZibraLiquid::SetMeshOptimizationIterations(int NewValue)
{
	RenderingParameters.MeshOptimizationIterations = NewValue;
}

float AZibraLiquid::GetVertexOptimizationStep()
{
	return RenderingParameters.VertexOptimizationStep;
}

void AZibraLiquid::SetVertexOptimizationStep(float NewValue)
{
	RenderingParameters.VertexOptimizationStep = NewValue;
}

float AZibraLiquid::GetMeshOptimizationStep()
{
	return RenderingParameters.MeshOptimizationStep;
}

void AZibraLiquid::SetMeshOptimizationStep(float NewValue)
{
	RenderingParameters.MeshOptimizationStep = NewValue;
}

float AZibraLiquid::GetDualContourIsoSurfaceLevel()
{
	return RenderingParameters.DualContourIsoSurfaceLevel;
}

void AZibraLiquid::SetDualContourIsoSurfaceLevel(float NewValue)
{
	RenderingParameters.DualContourIsoSurfaceLevel = NewValue;
}

float AZibraLiquid::GetIsoSurfaceLevel()
{
	return RenderingParameters.IsoSurfaceLevel;
}

void AZibraLiquid::SetIsoSurfaceLevel(float NewValue)
{
	RenderingParameters.IsoSurfaceLevel = NewValue;
}

float AZibraLiquid::GetRayMarchIsoSurface()
{
	return RenderingParameters.RayMarchIsoSurface;
}

void AZibraLiquid::SetRayMarchIsoSurface(float NewValue)
{
	RenderingParameters.RayMarchIsoSurface = NewValue;
}

int AZibraLiquid::GetRayMarchMaxSteps()
{
	return RenderingParameters.RayMarchMaxSteps;
}

void AZibraLiquid::SetRayMarchMaxSteps(float NewValue)
{
	RenderingParameters.RayMarchMaxSteps = NewValue;
}

float AZibraLiquid::GetRayMarchStepSize()
{
	return RenderingParameters.RayMarchStepSize;
}

void AZibraLiquid::SetRayMarchStepSize(float NewValue)
{
	RenderingParameters.RayMarchStepSize = NewValue;
}

float AZibraLiquid::GetRayMarchStepFactor()
{
	return RenderingParameters.RayMarchStepFactor;
}

void AZibraLiquid::SetRayMarchStepFactor(float NewValue)
{
	RenderingParameters.RayMarchStepFactor = NewValue;
}

void AZibraLiquid::Init(ZibraLiquidProxy* Proxy, FRegisterSolverBuffersBridgeParams RegisterSolverBuffersBridgeParams, FRegisterParticlesBuffersBridgeParams RegisterParticlesBuffersParams, FRegisterManipulatorsBridgeParams RegisterManipulatorsBridgeParams, FInitializeGPUReadbackParams InitializeGPUReadbackParams)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ZibraLiquid_BeginPlay_Init);

	check(IsInRenderingThread() || IsInRHIThread());

#if !WITH_ZIBRA_AI
	UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("AZibraLiquid can not be initialized. Make sure Zibra DLL is loaded"));
	return;
#endif

	ZibraLiquidBridge::CreateLiquidInstance(Proxy->InstanceID);

	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);
	//InitializeNativeResources
	ZibraLiquidBridge::RegisterParticlesBuffers(Proxy->InstanceID, &RegisterParticlesBuffersParams);
	ZibraLiquidBridge::RegisterSolverBuffers(Proxy->InstanceID, &RegisterSolverBuffersBridgeParams);

	//InitializeManipulators
	ZibraLiquidBridge::RegisterManipulators(Proxy->InstanceID, &RegisterManipulatorsBridgeParams);
	
	ZibraLiquidBridge::InitializeGpuReadback(Proxy->InstanceID, &InitializeGPUReadbackParams);
	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);

}

void AZibraLiquid::UpdateRegisterParticlesBuffersParams()
{
	RegisterParticlesBuffersParams.PositionMass = GetNativePtr(Proxy->PositionMassRHI);
	RegisterParticlesBuffersParams.AffineVelocity0 = GetNativePtr(Proxy->Affine0RHI);
	RegisterParticlesBuffersParams.AffineVelocity1 = GetNativePtr(Proxy->Affine1RHI);
	RegisterParticlesBuffersParams.PositionRadius = GetNativePtr(Proxy->PositionRadiusRHI);
	RegisterParticlesBuffersParams.ParticleNumber = GetNativePtr(Proxy->ParticleNumberRHI);
}

void AZibraLiquid::UpdateRegisterSolverBuffersBridgeParams()
{
	RegisterSolverBuffersBridgeParams.SimulationParams = &FluidParameters;
	RegisterSolverBuffersBridgeParams.PositionMassCopy = GetNativePtr(Proxy->PositionMassCopyRHI);
	RegisterSolverBuffersBridgeParams.GridData = GetNativePtr(Proxy->GridDataRHI);
	RegisterSolverBuffersBridgeParams.IndexGrid = GetNativePtr(Proxy->IndexGridRHI);
	RegisterSolverBuffersBridgeParams.GridBlur0 = GetNativePtr(Proxy->GridBlur0RHI);
	RegisterSolverBuffersBridgeParams.GridBlur1 = GetNativePtr(Proxy->GridBlur1RHI);
	RegisterSolverBuffersBridgeParams.GridNormal = GetNativePtr(Proxy->GridNormalRHI);
	RegisterSolverBuffersBridgeParams.GridSDF = GetNativePtr(Proxy->GridSDFRHI);
	RegisterSolverBuffersBridgeParams.NodeParticlePairs0 = GetNativePtr(Proxy->NodeParticlePairs0RHI);
	RegisterSolverBuffersBridgeParams.NodeParticlePairs1 = GetNativePtr(Proxy->NodeParticlePairs1RHI);
	RegisterSolverBuffersBridgeParams.RadixGroupData1 = GetNativePtr(Proxy->RadixGroupData1RHI);
	RegisterSolverBuffersBridgeParams.RadixGroupData2 = GetNativePtr(Proxy->RadixGroupData2RHI);
	RegisterSolverBuffersBridgeParams.RadixGroupData3 = GetNativePtr(Proxy->RadixGroupData3RHI);
	RegisterSolverBuffersBridgeParams.Counters = GetNativePtr(Proxy->CountersRHI);
	RegisterSolverBuffersBridgeParams.VertexIDGrid = GetNativePtr(Proxy->VertexIDGridRHI);
	RegisterSolverBuffersBridgeParams.VertexBuffer0 = GetNativePtr(Proxy->VertexBuffer0RHI);
	RegisterSolverBuffersBridgeParams.VertexBuffer1 = GetNativePtr(Proxy->VertexBuffer1RHI);
	RegisterSolverBuffersBridgeParams.QuadBuffer = GetNativePtr(Proxy->QuadBufferRHI);
	RegisterSolverBuffersBridgeParams.GridDensity = Proxy->DensityTextureBridge;
	RegisterSolverBuffersBridgeParams.GridNormals = Proxy->GridNormalTextureBridge;
	RegisterSolverBuffersBridgeParams.TransferDataBuffer = GetNativePtr(Proxy->TransferDataBufferRHI);
	RegisterSolverBuffersBridgeParams.MeshRenderIndexBuffer = GetNativePtr(Proxy->MeshRenderIndexBufferRHI);

	RegisterSolverBuffersBridgeParams.NativeMeshVertexBuffer = nullptr;
	RegisterSolverBuffersBridgeParams.NativeMeshIndexBuffer = nullptr;
	RegisterSolverBuffersBridgeParams.VertexData = GetNativePtr(Proxy->VertexPropertiesRHI);
}

void AZibraLiquid::RegisterRenderThreadCallback()
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateUObject(this, &AZibraLiquid::RenderCallBack));
	}
}

void AZibraLiquid::RemoveDestroyedManipulator(UZibraManipulator* Manipulator)
{
	if (manipulators.Contains(Manipulator))
	{
		manipulators.Remove(Manipulator);
		ManipulatorManager->RemoveManipulator(Manipulator);
	}
}

void AZibraLiquid::RemoveDestroyedCollider(UZibraSDFCollider* Collider)
{
	if (ZibraSDFColliders.Contains(Collider))
	{
		ZibraSDFColliders.Remove(Collider);
		ManipulatorManager->RemoveCollider(Collider);
	}
}

bool AZibraLiquid::IsRenderingEnabled()
{
	return bInitialized && bRunRendering && (SimulationInternalFrame > 1);
}
void AZibraLiquid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CubeComponent->SetMaterial(0, NULL);

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	ManipulatorManager = nullptr;

	ReleaseGPUResources();
	OnPostResolvedSceneColorHandle.Reset();
	ZibraNotifications::ResetShownMessages();
}

FVector AZibraLiquid::GetContainerPosition()
{
	return ContainerPosition;
}

void AZibraLiquid::RenderCallBack(FPostOpaqueRenderParameters& Parameters)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ZibraLiquid_RenderCallBack);

	if (!bInitialized || !bIsEnabled || !IsRenderingEnabled())
		return;


#if ENGINE_MAJOR_VERSION == 4
	FRHICommandListImmediate& RHICmdList = *Parameters.RHICmdList;
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ZibraLiquid_PixelShader); // Used to gather CPU profiling data for the UE4 session frontend
	SCOPED_DRAW_EVENT(RHICmdList, ZibraLiquid); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc
	SCOPED_GPU_STAT(RHICmdList, Stat_Zibra_Liquid_Render);

	UpdateCamera(Parameters);
	UpdateNativeTextures();
	UpdateNativeRenderParams(Parameters);

	DisableUAVOverlap(RHICmdList);

	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);
	ZibraLiquidBridge::Draw(Proxy->InstanceID);
	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);

#elif ENGINE_MAJOR_VERSION == 5

	FRDGBuilder* GraphBuilder = Parameters.GraphBuilder;
	FRHICopyTextureInfo CopyInfo;

	UpdateCamera(Parameters);
	UpdateNativeTextures();
	UpdateNativeRenderParams(Parameters);

	ZibraLiquidProxy* ProxyCopy = Proxy;
	ENQUEUE_RENDER_COMMAND(Draw)(
		[ProxyCopy](FRHICommandListImmediate& RHICmdList)
		{
			SCOPED_DRAW_EVENT(RHICmdList, ZibraLiquid);
			SCOPED_GPU_STAT(RHICmdList, Stat_Zibra_Liquid_Render);
			ZibraLiquidBridge::SaveAPIState(ProxyCopy->InstanceID);
			ZibraLiquidBridge::Draw(ProxyCopy->InstanceID);
			ZibraLiquidBridge::RestoreAPIState(ProxyCopy->InstanceID);
		});

#endif

}

void AZibraLiquid::SimulationCallBack(ZibraLiquidProxy* Proxy, FRHICommandListImmediate& RHICmdList, int IterationsPerFrameCopy, FSimulationParams FluidParametersCopy, const TArray<FManipulatorParams>& ManipulatorParamsCopy, int ManipulatorManagerElements)
{
	check(IsInRenderingThread());
	SCOPED_GPU_STAT(RHICmdList, Stat_Zibra_Liquid_SimulatePhysics);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ZibraLiquid_SimulationCallBack);
	SCOPED_DRAW_EVENT(RHICmdList, ZibraLiquidStepPhysics); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	DisableUAVOverlap(RHICmdList);

	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);
	StepPhysics(Proxy, IterationsPerFrameCopy, FluidParametersCopy, ManipulatorParamsCopy, ManipulatorManagerElements);

	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);
}

void AZibraLiquid::UpdateReadback()
{


}

float AZibraLiquid::GetCellSize()
{
	return CellSize;
}

void AZibraLiquid::AddAllColliders()
{
	TArray<AActor*> Colliders;
	for (TObjectIterator<UZibraSDFCollider> Itr; Itr; ++Itr)
	{
		//Check, because TObjectIterator finds objects in the editor as well as the PIE world, in an indeterminate order.
		if (Itr->GetWorld() == GetWorld())
		{
			if (!ZibraSDFColliderActors.Contains(Itr->GetOwner()))
			{
				ZibraSDFColliderActors.Add(Itr->GetOwner());
			}
		}
	}
	TArray<AActor*> ActorsToRemove;
	for (AActor* actor : ZibraSDFColliderActors)
	{
		if (actor == nullptr)
		{
			ActorsToRemove.Add(actor);
		}
	}

	for (AActor* actor : ActorsToRemove)
	{
		ZibraSDFColliderActors.Remove(actor);
	}
}

void AZibraLiquid::AddAllManipulators()
{
	TArray<AActor*> Manipulators;
	for (TObjectIterator<UZibraManipulator> Itr; Itr; ++Itr)
	{

		//Check, because TObjectIterator finds objects in the editor as well as the PIE world, in an indeterminate order.
		if (!Itr->IsA(UZibraSDFCollider::StaticClass()) && Itr->GetWorld() == GetWorld())
		{
			if (!ManipulatorActors.Contains(Itr->GetOwner()))
			{
				ManipulatorActors.Add(Itr->GetOwner());
			}
		}
	}

	TArray<AActor*> ActorsToRemove;
	for (AActor* actor : ManipulatorActors)
	{
		if (actor == nullptr)
		{
			ActorsToRemove.Add(actor);
		}
	}

	for (AActor* actor : ActorsToRemove)
	{
		ManipulatorActors.Remove(actor);
	}
}

FVector AZibraLiquid::GetGravity()
{

	return SolverParameters.Gravity;
}

void AZibraLiquid::SetGravity(FVector NewValue)
{
	SolverParameters.Gravity = NewValue;
}

float AZibraLiquid::GetFluidStiffness()
{
	return SolverParameters.FluidStiffness;
}

void AZibraLiquid::SetFluidStiffness(float NewValue)
{
	SolverParameters.FluidStiffness = NewValue;
}

float AZibraLiquid::GetParticleDensity()
{
	return SolverParameters.ParticleDensity;
}

void AZibraLiquid::SetParticleDensity(float NewValue)
{
	SolverParameters.ParticleDensity = NewValue;
}

float AZibraLiquid::GetMaximumVelocity()
{
	return SolverParameters.MaximumVelocity;
}

void AZibraLiquid::SetMaximumVelocity(float NewValue)
{
	SolverParameters.MaximumVelocity = NewValue;
}

float AZibraLiquid::GetMinimumVelocity()
{
	return SolverParameters.MinimumVelocity;
}

void AZibraLiquid::SetMinimumVelocity(float NewValue)
{
	SolverParameters.MinimumVelocity = NewValue;
}

float AZibraLiquid::GetViscosity()
{
	return SolverParameters.Viscosity;
}

void AZibraLiquid::SetViscosity(float NewValue)
{
	SolverParameters.Viscosity = NewValue;
}

float AZibraLiquid::GetSurfaceTension()
{
	return SolverParameters.SurfaceTension;
}

void AZibraLiquid::SetSurfaceTension(float NewValue)
{
	SolverParameters.SurfaceTension = NewValue;
}

float AZibraLiquid::GetForceInteractionStrength()
{
	return SolverParameters.ForceInteractionStrength;
}

void AZibraLiquid::SetForceInteractionStrength(float NewValue)
{
	SolverParameters.ForceInteractionStrength = NewValue;
}

void AZibraLiquid::RunSimulation()
{
	bRunSimulation = true;
}

void AZibraLiquid::StopSimulation()
{
	bRunSimulation = false;
}

bool AZibraLiquid::SDFVisualizationRunning()
{
	return bVisualizeSceneSDF;
}

void AZibraLiquid::RunSDFVisualization()
{
	bVisualizeSceneSDF = true;
}

void AZibraLiquid::StopSDFVisualization()
{
	bVisualizeSceneSDF = false;
}

void AZibraLiquid::UpdateCamera(FPostOpaqueRenderParameters& Parameters)
{
	FIntRect ViewportRect = Parameters.ViewportRect;
	FMatrix InverseView = Parameters.ViewMatrix.InverseFast();
	CameraRenderParams.WorldSpaceCameraPos = ZibraVector3(InverseView.GetTransposed().GetColumn(3).X, InverseView.GetTransposed().GetColumn(3).Y, InverseView.GetTransposed().GetColumn(3).Z);

	CameraRenderParams.View = (ZibraMatrix)Parameters.ViewMatrix;
	CameraRenderParams.Projection = (ZibraMatrix)Parameters.ProjMatrix;
	CameraRenderParams.ProjectionInverse = (ZibraMatrix)CameraRenderParams.Projection.InverseFast();
	CameraRenderParams.ViewProjection = (ZibraMatrix)CameraRenderParams.View * CameraRenderParams.Projection;
	CameraRenderParams.ViewProjectionInverse = (ZibraMatrix)CameraRenderParams.ViewProjection.InverseFast();

#if ENGINE_MAJOR_VERSION == 4 
	CameraRenderParams.CameraResolution = ZibraVector2(Parameters.DepthTexture->GetSizeX(), Parameters.DepthTexture->GetSizeY());
#elif ENGINE_MAJOR_VERSION == 5
	CameraRenderParams.CameraResolution = ZibraVector2(Parameters.DepthTexture->Desc.Extent.X, Parameters.DepthTexture->Desc.Extent.Y);
#endif
	
	CameraRenderParams.CameraID = 0;
	CameraRenderParams.EyeRayCameraCoeficients = CalculateEyeRayCameraCoeficients((ZibraMatrix)Parameters.ViewMatrix);

	MeshRenderGlobalParams.LiquidIOR = MaterialParameters.IndexOfRefraction;
	MeshRenderGlobalParams.RayMarchIsoSurface = RenderingParameters.RayMarchIsoSurface;
	MeshRenderGlobalParams.ContainerPosition = (ZibraVector3)ContainerPosition;
	MeshRenderGlobalParams.RayMarchMaxSteps = RenderingParameters.RayMarchMaxSteps;
	MeshRenderGlobalParams.RayMarchStepSize = RenderingParameters.RayMarchStepSize;
	MeshRenderGlobalParams.RayMarchStepFactor = RenderingParameters.RayMarchStepFactor;
	MeshRenderGlobalParams.UnderwaterRender = RenderingParameters.bUnderwaterRender ? 1 : 0;

	FIntPoint renderingResolution = ApplyDownscaleFactor(FIntPoint(CameraRenderParams.CameraResolution.X, CameraRenderParams.CameraResolution.Y));
	FIntPoint rayMarchResolution = renderingResolution * RenderingParameters.RayMarchingResolutionDownscale;
	MeshRenderGlobalParams.RayMarchResolution = FIntPoint((int)rayMarchResolution.X, (int)rayMarchResolution.Y);

	switch (RenderingParameters.RefractionBounces)
	{
	case ERayMarchingBounces_SingleBounce:
		MeshRenderGlobalParams.TwoBouncesEnabled = 0;
		break;
	case ERayMarchingBounces_TwoBounces:
		MeshRenderGlobalParams.TwoBouncesEnabled = 1;
		break;
	default:
		break;
	}

	ZibraLiquidProxy* ProxyCopy = Proxy;
	FCameraParams CameraRenderParamsCopy = CameraRenderParams;
	FMeshRenderGlobalParams MeshRenderGlobalParamsCopy = MeshRenderGlobalParams;
	ENQUEUE_RENDER_COMMAND(UpdateCameraRenderParamsRT)(
		[ProxyCopy, CameraRenderParamsCopy, MeshRenderGlobalParamsCopy](FRHICommandListImmediate& RHICmdList)
		{
			UpdateCameraRenderParamsRT(ProxyCopy, CameraRenderParamsCopy, MeshRenderGlobalParamsCopy);
		});
}

FIntPoint AZibraLiquid::ApplyDownscaleFactor(FIntPoint val)
{
	if (!bEnableDownscale)
		return val;
	return FIntPoint((int)(val.X * DownscaleFactor), (int)(val.Y * DownscaleFactor));
}

void AZibraLiquid::UpdateNativeTextures()
{
	const int32 Width = FMath::Max((int32)CameraRenderParams.CameraResolution.X, 1);
	const int32 Height = FMath::Max((int32)CameraRenderParams.CameraResolution.Y, 1);

	if (OldWidth != Width || OldHeight != Height)
	{
		bIsDirty = true;
		bNeedRegister2dTextures = true;
	}

	OldWidth = Width > 1 ? Width : 0;
	OldHeight = Height > 1 ? Height : 0;


	if (bIsDirty || bNeedRegister2dTextures)
	{
		int32 NumElements = Width * Height;
		{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 26
			{
				Color0UT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_A32B32G32R32F, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable | TexCreate_Shared, TEXT("Color0UT"));
				Proxy->Color0Bridge = GetTextureBridgePtr(Color0UT->Resource->GetTexture2DRHI());
			}
			{
				DepthUT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_DepthStencil, TexCreate_ShaderResource | TexCreate_DepthStencilTargetable, TEXT("DepthUT"));
				Proxy->DepthBridge = GetTextureBridgePtr(DepthUT->Resource->GetTexture2DRHI());
			}
			{
				Color1UT = UZibraTexture2DDynamic::Create(Width, Height, EPixelFormat::PF_A32B32G32R32F, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable | TexCreate_Shared, TEXT("Color1UT"));
				Proxy->Color1Bridge = GetTextureBridgePtr(Color1UT->Resource->GetTexture2DRHI());
			}
#elif ENGINE_MAJOR_VERSION == 5
#endif
			{
				SDFRenderRT->SizeX = Width;
				SDFRenderRT->SizeY = Height;
				SDFRenderRT->bCanCreateUAV = true;
				SDFRenderRT->OverrideFormat = EPixelFormat::PF_FloatRGBA;
				SDFRenderRT->GetRenderTargetResource()->ReleaseRHI();
				SDFRenderRT->GetRenderTargetResource()->ReleaseDynamicRHI();
				SDFRenderRT->GetRenderTargetResource()->InitDynamicRHI();
				Proxy->SDFRenderBridge = GetTextureBridgePtr(SDFRenderRT->GetRenderTargetResource()->GetTexture2DRHI());
			}

			if (b3DTexturesDirty)
			{
				RenderTargetGridNormals->SizeX = GridSize.X;
				RenderTargetGridNormals->SizeY = GridSize.Y;
				RenderTargetGridNormals->SizeZ = GridSize.Z;
				RenderTargetGridNormals->bCanCreateUAV = true;
				RenderTargetGridNormals->OverrideFormat = EPixelFormat::PF_A16B16G16R16;

				RenderTargetGridDensity->SizeX = GridSize.X;
				RenderTargetGridDensity->SizeY = GridSize.Y;
				RenderTargetGridDensity->SizeZ = GridSize.Z;
				RenderTargetGridDensity->OverrideFormat = PF_R32_FLOAT;
				RenderTargetGridDensity->bCanCreateUAV = true;

				b3DTexturesDirty = false;
			}
		}

		Proxy->AtomicGridRHI.SafeRelease();
		Proxy->JFAGrid0RHI.SafeRelease();
		Proxy->JFAGrid1RHI.SafeRelease();

		{
			int32 ElementSize = sizeof(uint32);
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("AtomicGridRHI"));
			Proxy->AtomicGridRHI = RHICreateStructuredBuffer(ElementSize, 2 * ElementSize * NumElements, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}
		{

			int32 ElementSize = sizeof(uint32);
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("JFAGrid0RHI"));
			Proxy->JFAGrid0RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumElements, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}
		{
			int32 ElementSize = sizeof(uint32);
			FRHIResourceCreateInfo CreateInfo = GetCreateInfo(TEXT("JFAGrid1RHI"));
			Proxy->JFAGrid1RHI = RHICreateStructuredBuffer(ElementSize, ElementSize * NumElements, BUF_UnorderedAccess | BUF_ShaderResource | BUF_StructuredBuffer, CreateInfo);
		}

		bIsDirty = false;
		
		const int CameraID = 0;

		RegisterRenderResourcesBridgeParams.Depth = Proxy->DepthBridge;
		RegisterRenderResourcesBridgeParams.Color0 = Proxy->Color0Bridge;
		RegisterRenderResourcesBridgeParams.Color1 = Proxy->Color1Bridge;
		RegisterRenderResourcesBridgeParams.AtomicGrid = GetNativePtr(Proxy->AtomicGridRHI);
		RegisterRenderResourcesBridgeParams.JFA0 = GetNativePtr(Proxy->JFAGrid0RHI);
		RegisterRenderResourcesBridgeParams.JFA1 = GetNativePtr(Proxy->JFAGrid1RHI);
		RegisterRenderResourcesBridgeParams.SDFRender = Proxy->SDFRenderBridge;

		FRegisterRenderResourcesBridgeParams RegisterRenderResourcesBridgeParamsCopy = RegisterRenderResourcesBridgeParams;

		ZibraLiquidProxy* ProxyCopy = Proxy;

		ENQUEUE_RENDER_COMMAND(RegisterRenderResourcesRT)(
			[ProxyCopy, RegisterRenderResourcesBridgeParamsCopy](FRHICommandListImmediate& RHICmdList)
			{
				RegisterRenderResourcesRT(ProxyCopy, RegisterRenderResourcesBridgeParamsCopy);
			});
		bNeedRegister2dTextures = false;
	}

}

void AZibraLiquid::UpdateNativeRenderParams(FPostOpaqueRenderParameters& Parameters)
{
	ParticleDiameter = MaterialParameters.ParticleScale * CellSize /
		(float)FMath::Pow(SolverParameters.ParticleDensity, 0.333f);

	RenderParams.BlurRadius = MaterialParameters.BlurRadius;
	RenderParams.Diameter = ParticleDiameter;
	RenderParams.NeuralSamplingDistance = MaterialParameters.NeuralSamplingDistance;
	RenderParams.SDFDebug = MaterialParameters.SDFDebug;
	RenderParams.RenderingMode = int(ERenderingMode::ERenderingMode_MeshRender);
	RenderParams.VertexOptimizationIterations = RenderingParameters.VertexOptimizationIterations;
	RenderParams.MeshOptimizationIterations = RenderingParameters.MeshOptimizationIterations;
	RenderParams.JFAIterations = RenderingParameters.AdditionalJFAIterations;
	RenderParams.DualContourIsoValue = RenderingParameters.DualContourIsoSurfaceLevel;
	RenderParams.MeshOptimizationStep = RenderingParameters.MeshOptimizationStep;


	int MaxVertexCount = NumNodes;
	int MaxTriangleCount = (int)(MaxVertexCount * RenderingParameters.MaxLiquidMeshSize / 3.0f);

	RenderParams.MaxVertexBufferSize = MaxTriangleCount * 6;
	RenderParams.MaxIndexBufferSize = MaxTriangleCount * 3;

	ZibraLiquidProxy* ProxyCopy = Proxy;
	FRenderParams RenderParamsCopy = RenderParams;
	ENQUEUE_RENDER_COMMAND(SetNativeRenderParamsRT)(
		[ProxyCopy, RenderParamsCopy](FRHICommandListImmediate& RHICmdList)
		{
			SetNativeRenderParamsRT(ProxyCopy, RenderParamsCopy);
		});
}

void AZibraLiquid::StepPhysics(ZibraLiquidProxy* Proxy, int IterationsPerFrame, FSimulationParams FluidParameters, const TArray<FManipulatorParams>& ManipulatorParams, int ManipulatorManagerElements)
{
	ZibraLiquidBridge::GarbageCollect();

	for (int32 i = 0; i < IterationsPerFrame; i++)
	{
		ZibraLiquidBridge::ClearSDFAndIDGrid(Proxy->InstanceID);

		ZibraLiquidBridge::UpdateLiquidParameters(Proxy->InstanceID, &FluidParameters);

		if (ManipulatorManagerElements > 0)
		{
			// Update manipulator parameters
			ZibraLiquidBridge::UpdateManipulatorParameters(Proxy->InstanceID, static_cast<const void*>(ManipulatorParams.GetData()));
		}

		// execute simulation
		ZibraLiquidBridge::StepPhysics(Proxy->InstanceID);
	}
}

void AZibraLiquid::UpdateMaterial()
{
	if (FluidMaterial)
	{
		FluidMaterial->SetScalarParameterValue("Index Of Refraction", MaterialParameters.IndexOfRefraction);
		FluidMaterial->SetScalarParameterValue("Metallic", MaterialParameters.Metalness);
		FluidMaterial->SetScalarParameterValue("Opacity", MaterialParameters.Opacity);
		FluidMaterial->SetScalarParameterValue("Roughness", MaterialParameters.Roughness);
		FluidMaterial->SetScalarParameterValue("Absorption", MaterialParameters.AbsorptionAmount);
		FluidMaterial->SetScalarParameterValue("Scattering", MaterialParameters.ScatteringAmount);
		FluidMaterial->SetScalarParameterValue("SpecularIntensity", MaterialParameters.SpecularIntensity);
		FluidMaterial->SetScalarParameterValue("FresnelStrength", MaterialParameters.FresnelStrength);
		FluidMaterial->SetScalarParameterValue("EmmisiveMultiplier", MaterialParameters.EmmisiveMultiplier);


		FluidMaterial->SetVectorParameterValue("Color", MaterialParameters.Color);
		FluidMaterial->SetVectorParameterValue("ReflectionColor", MaterialParameters.ReflectionColor);
		FluidMaterial->SetVectorParameterValue("EmissiveColor", MaterialParameters.EmissiveColor);

		FluidMaterial->SetTextureParameterValue("MeshDepth", DepthUT);
		FluidMaterial->SetTextureParameterValue("MeshRenderData", Color0UT);
		FluidMaterial->SetTextureParameterValue("RayMarchData", Color1UT);
		FluidMaterial->SetTextureParameterValue("GridDensity", RenderTargetGridDensity);
		FluidMaterial->SetTextureParameterValue("GridNormals", RenderTargetGridNormals);	
		FluidMaterial->SetTextureParameterValue("ReflectionCapture", ReflectionCubemap);

		
	}
}

ZibraMatrix AZibraLiquid::CalculateEyeRayCameraCoeficients(ZibraMatrix ViewMatrix)
{
	if (GetWorld() || GetWorld()->GetFirstPlayerController())
	{
		const APlayerController* FP = GetWorld()->GetFirstPlayerController();
		if (FP)
		{
			const APlayerCameraManager* cam = FP->PlayerCameraManager;
			float fov = cam->GetFOVAngle();
			float fovTan = FMath::Tan(FMath::DegreesToRadians(fov * 0.5f));

			float AspectRatio = CameraRenderParams.CameraResolution.X / CameraRenderParams.CameraResolution.Y;

			ZibraVector3 forward = (ZibraVector3)ViewMatrix.GetColumn(1);
			ZibraVector3 right = (ZibraVector3)ViewMatrix.GetColumn(2);
			ZibraVector3 up = (ZibraVector3)ViewMatrix.GetColumn(0);


			ZibraVector3 r = right * AspectRatio * fovTan;
			ZibraVector3 u = -up * fovTan;
			ZibraVector3 v = forward;

			ZibraMatrix ret = ZibraMatrix();
			ret.SetColumn(0, ZibraVector4(r.X, r.Y, r.Z, 0.0f));
			ret.SetColumn(1, ZibraVector4(u.X, u.Y, u.Z, 0.0f));
			ret.SetColumn(2, ZibraVector4(v.X, v.Y, v.Z, 0.0f));
			ret.SetColumn(3, ZibraVector4(0.0f, 0.0f, 0.0f, 0.0f));

			return ret.GetTransposed();
		}
	}
	return ZibraMatrix();
}

int32 AZibraLiquid::GetParticleCountRounded()
{
	return (int32)FMath::CeilToInt((double)MaxNumParticles / MPM_THREADS) * MPM_THREADS; // round to workgroup size;
}

ULONG AZibraLiquid::GetParticleCountFootprint()
{
	ULONG result = 0;
	int particleCountRounded = GetParticleCountRounded();
	result += (ULONG)(MaxNumParticles * 4 * sizeof(float));            // PositionMass
	result += (ULONG)(MaxNumParticles * 4 * sizeof(float));            // PositionRadius
	result += (ULONG)(2 * 4 * particleCountRounded * 2 * sizeof(int)); // Affine
	result += (ULONG)(particleCountRounded * 4 * sizeof(float));       // positionMassCopy
	result += (ULONG)(particleCountRounded * 2 * sizeof(int));         // nodeParticlePairs

	result += (ULONG)(particleCountRounded * 2 * sizeof(int));                 // SortTempBuffer
	result += (ULONG)((RadixWorkGroups + 1) * 3 * HISTO_WIDTH * sizeof(int)); // RadixGroupData

	return result;
}

ULONG AZibraLiquid::GetCollidersFootprint()
{
	ULONG result = 0;

	for (AActor* ColliderActor : ZibraSDFColliderActors)
	{
		if (ColliderActor)
		{
			UZibraSDFCollider* Collider = ColliderActor->FindComponentByClass < UZibraSDFCollider>();
			if (Collider)
			{
				result += Collider->GetMemoryFootrpint();
			}
		}
	}

	// At least this part is wrong
	int ManipSize = 0;
	if (ManipulatorManager)
		ManipSize = ManipulatorManager->ManipulatorParams.GetAllocatedSize() + ManipulatorManager->ManipulatorParams.GetTypeSize() * ManipulatorManager->ManipulatorParams.Num();

	result += (ULONG)(manipulators.Num() * ManipSize);   // DynamicManipData
	result += (ULONG)(manipulators.Num() * sizeof(int)); // ConstManipData

	return result;
}

ULONG AZibraLiquid::GetGridFootprint()
{
	ULONG result = 0;

	FIntVector GridSizeVector = FIntVector(
		FMath::CeilToInt(ContainerSize.X / CellSize),
		FMath::CeilToInt(ContainerSize.Y / CellSize),
		FMath::CeilToInt(ContainerSize.Z / CellSize));

	int NumNodesLocal = GridSizeVector[0] * GridSizeVector[1] * GridSizeVector[2];

	result += (ULONG)(NumNodesLocal * 4 * sizeof(int));                        // GridData
	result += (ULONG)(NumNodesLocal * 4 * sizeof(float));                      // GridNormal
	result += (ULONG)(NumNodesLocal * sizeof(float));                          // GridBlur0
	result += (ULONG)(NumNodesLocal * sizeof(float));                          // GridBlur1
	result += (ULONG)(NumNodesLocal * sizeof(float));                          // GridSDF
	result += (ULONG)(NumNodesLocal * 2 * sizeof(int));                        // IndexGrid
	result += (ULONG)(NumNodesLocal * sizeof(int));                            // VertexIDGrid
	result += (ULONG)(NumNodesLocal * 4 * sizeof(float));					   // VertexBuffer
	result += (ULONG)(NumNodesLocal * sizeof(int));							   // QuadBuffer
	result += (ULONG)(NumNodesLocal * (sizeof(int) * 4));					   // VertexProperties
	result += (ULONG)(NumNodesLocal * 2 * sizeof(float));                      // GridNormalTexture
	result += (ULONG)(NumNodesLocal * sizeof(float) / 2);                      // DensityTexture

	return result;
}

void AZibraLiquid::UpdateCameraRenderParamsRT(ZibraLiquidProxy* Proxy, FCameraParams CameraRenderParams, FMeshRenderGlobalParams MeshRenderGlobalParams)
{
	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);

	ZibraLiquidBridge::SetCameraParameters(Proxy->InstanceID, &CameraRenderParams);
	ZibraLiquidBridge::UpdateMeshRenderGlobalParameters(Proxy->InstanceID, &MeshRenderGlobalParams);
	
	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);
}

void AZibraLiquid::RegisterRenderResourcesRT(ZibraLiquidProxy* Proxy, FRegisterRenderResourcesBridgeParams RegisterRenderResourcesBridgeParams)
{
	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);

	ZibraLiquidBridge::RegisterRenderResources(Proxy->InstanceID, &RegisterRenderResourcesBridgeParams);
	ZibraLiquidBridge::InitializeGraphicsPipeline(Proxy->InstanceID);

	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);
}

void AZibraLiquid::SetNativeRenderParamsRT(ZibraLiquidProxy* Proxy, FRenderParams RenderParams)
{
	ZibraLiquidBridge::SaveAPIState(Proxy->InstanceID);

	ZibraLiquidBridge::SetRenderParameters(Proxy->InstanceID, &RenderParams);

	ZibraLiquidBridge::RestoreAPIState(Proxy->InstanceID);
}

bool AZibraLiquid::CheckDll()
{
	const FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("ZibraAILiquid"))->GetBaseDir();
	const FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("ZibraFluidNative/Release"));
	const FString DLLPath = FPaths::Combine(*LibraryPath, TEXT("ZibraFluidNative_Win.dll"));

	return FPaths::FileExists(DLLPath);
}

void AZibraLiquid::ValidateGravity()
{
	const float GRAVITY_THRESHOLD = 100;

	SolverParameters.Gravity.X = FMath::Clamp(SolverParameters.Gravity.X, -GRAVITY_THRESHOLD, GRAVITY_THRESHOLD);
	SolverParameters.Gravity.X = FMath::Clamp(SolverParameters.Gravity.X, -GRAVITY_THRESHOLD, GRAVITY_THRESHOLD);
	SolverParameters.Gravity.Z = FMath::Clamp(SolverParameters.Gravity.Z, -GRAVITY_THRESHOLD, GRAVITY_THRESHOLD);
}

void AZibraLiquid::DisableUAVOverlap(FRHICommandListImmediate& RHICmdList)
{
	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		RHICmdList.Transition(FRHITransitionInfo((FRHIUnorderedAccessView*)0x1, ERHIAccess::UAVMask, ERHIAccess::UAVMask));
		RHICmdList.ImmediateFlush(EImmediateFlushType::DispatchToRHIThread);
	}
}

void AZibraLiquid::SpawnEmitterActor()
{
	if (!bSpawnedEmitterActor)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();
		AActor* Emitter = GetWorld()->SpawnActor(AZibraLiquidEmitterActor::StaticClass(), &Location, &Rotation);
		ManipulatorActors.Add(Emitter);
		Emitter->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		bSpawnedEmitterActor = true;
	}
}

float AZibraLiquid::DivergenceDecayCurve(float x)
{
	float a = (0.177f - 0.85f * x + 9.0f * x * x) / 1.8f;
	return 1.8f * a / (a + 1);
}

void AZibraLiquid::UpdateManipulatorStatistics()
{

}

void AZibraLiquid::EditorTick(float inDeltaTime)
{
#if WITH_EDITOR

	SpawnEmitterActor();

	SetActorScale3D(FVector(1, 1, 1));
	SetActorRotation(FQuat::MakeFromEuler(FVector(0, 0, 0)));

	CubeComponent->SetRelativeScale3D(ContainerSize);
	DrawDebugBox(GEditor->GetEditorWorldContext().World(), GetActorLocation(), ContainerSizeScaled * .5, GetActorRotation().Quaternion(), FColor::Yellow, false, 0, 0U, 2);

	ULONG ParticleCountFootprint = GetParticleCountFootprint() / (1 << 20);
	ParticleCountFootprintDisplay = FString::Printf(TEXT("%lu MB"), ParticleCountFootprint);

	ULONG GridFootprint = GetGridFootprint() / (1 << 20);
	GridSizeDisplay = FString::Printf(TEXT("%lu MB"), GridFootprint);

	if (MaxNumParticles % 256 != 0)
	{
		MaxNumParticles = 256 * (MaxNumParticles / 256);
	}

	
#endif
}

bool AZibraLiquid::ShouldTickIfViewportsOnly() const
{
	return true;
}

FVector AZibraLiquid::GetContainerSize()
{
	return ContainerSize;
}

void AZibraLiquid::SetContainerSize(FVector NewValue)
{
	if (!bInitialized)
		ContainerSize = NewValue;
	else 
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not set ContainerSize after Liquid is initialized. Please set it before initialization."));

}

void AZibraLiquid::AddZibraSDFCollider(UZibraSDFCollider* NewCollider)
{
	if (!bInitialized)
		ZibraSDFColliders.Add(NewCollider);
	else
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not add SDFCollider after Liquid is initialized. Please do it before initialization."));
}

void AZibraLiquid::RemoveZibraSDFCollider(UZibraSDFCollider* NewCollider)
{
	if (!bInitialized)
		ZibraSDFColliders.Remove(NewCollider);
	else
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not remove SDFCollider after Liquid is initialized. Please do it before initialization."));
}

void AZibraLiquid::AddManipulator(UZibraManipulator* NewManipulator)
{
	if (!bInitialized)
		manipulators.Add(NewManipulator);
	else
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not add manipulator after Liquid is initialized. Please do it before initialization."));
}

void AZibraLiquid::RemoveManipulator(UZibraManipulator* Manipulator)
{
	if (!bInitialized)
		manipulators.Remove(Manipulator);
	else
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not remove manipulator after Liquid is initialized. Please do it before initialization."));
}

float AZibraLiquid::GetTimeStepMax()
{
	return TimeStepMax;
}

void AZibraLiquid::SetTimeStepMax(float NewValue)
{
	TimeStepMax = NewValue;
}

float AZibraLiquid::GetSimulationSpeed()
{
	return SimTimePerSec;
}

void AZibraLiquid::SetSimulationSpeed(float NewValue)
{
	SimTimePerSec = NewValue;
}

int AZibraLiquid::GetIterationsPerFrame()
{
	return IterationsPerFrame;
}

int32 AZibraLiquid::GetActiveParticleCount()
{
	return ActiveParticleCount;
}

void AZibraLiquid::SetIterationsPerFrame(int NewValue)
{
	IterationsPerFrame = NewValue;
}

float AZibraLiquid::GetMaxNumParticles()
{
	return MaxNumParticles;
}

void AZibraLiquid::SetMaxNumParticles(float NewValue)
{
	if (!bInitialized)
		MaxNumParticles = NewValue;
	else
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("Can not set MaxNumParticles after Liquid is initialized. Please set it before initialization."));

}

void AZibraLiquid::SetGridResolution(float NewValue)
{
	GridResolution = NewValue;
}

int AZibraLiquid::GetGridResolution()
{
	return GridResolution;
}

bool AZibraLiquid::SimulationRunning()
{
	return bRunSimulation;
}
