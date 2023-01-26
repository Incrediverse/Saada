// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraAILiquidRuntime.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Windows/WindowsPlatformProcess.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "ZibraLiquidBridge.h"
#include "ZibraLiquidDebug.h"
#include "ZibraLiquidEnums.h"
#include "ZibraLiquidTypes.h"

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

#define LOCTEXT_NAMESPACE "FZibraAILiquidRuntimeModule"

DEFINE_LOG_CATEGORY(LogZibraAILiquidRuntime);

void FZibraAILiquidRuntimeModule::StartupModule()
{
	const FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("ZibraAILiquid"))->GetBaseDir();
	const FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("ZibraFluidNative/Release"));
	const FString DLLPath = FPaths::Combine(*LibraryPath, TEXT("ZibraFluidNative_Win.dll"));
	
	FPlatformProcess::PushDllDirectory(*LibraryPath);
	Zibra_DLLHandle = !DLLPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*DLLPath): nullptr;

	if (Zibra_DLLHandle)
	{
		if(!ImportDLLMethods())
		{
			UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Failed to import methods from Zibra DLL"));
		}

		FString RHIName = GDynamicRHI->GetName();
		FUEInterface ueInterface;
		void* device = GDynamicRHI->RHIGetNativeDevice();

		if (RHIName == TEXT("D3D11"))
		{
			ueInterface.D3D11.Device = device;
			ZibraLiquidBridge::UnrealPluginLoad(&ueInterface, GfxRenderer::kGfxRendererD3D11);
		}
		else if (RHIName == TEXT("D3D12"))
		{
#if ENGINE_MAJOR_VERSION == 5
			FD3D12DynamicRHI* d3d12RHI = static_cast<FD3D12DynamicRHI*>(GDynamicRHI);
			FD3D12Device* unrealD3D12Device = d3d12RHI->GetAdapter().GetDevice(0);
			if (unrealD3D12Device == nullptr)
			{
				UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Can not get UE's D3D12 device. Zibra Liquids initialization failed."));
			}
			else
			{
				ID3D12CommandQueue* queue = unrealD3D12Device->GetCommandListManager().GetD3DCommandQueue();
				if (queue == nullptr)
				{
					UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Can not get D3D12 command queue. Zibra Liquids initialization failed."));
				}
				else
				{
					ueInterface.D3D12.Device = device;
					ueInterface.D3D12.Queue = queue;
					ZibraLiquidBridge::UnrealPluginLoad(&ueInterface, GfxRenderer::kGfxRendererD3D12);
				}
			}
#else
			if (RHIName == TEXT("D3D12"))
			{
				UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Unsupported graphic API. DirectX 12 is only supported in UE5. Please select DirectX 11 in Project Settings."));
			}
			else 
			{
				UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Unsupported graphic API. "));
			}
#endif
		}

	}
	else
	{
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Could not find the Zibra DLL, make sure it is located at %s!"), *LibraryPath);
	}
	FPlatformProcess::PopDllDirectory(*LibraryPath);
}

void FZibraAILiquidRuntimeModule::ShutdownModule()
{
	if (Zibra_DLLHandle)
	{
#if WITH_ZIBRA_AI
		ZibraLiquidBridge::CreateLiquidInstance = NULL;
		ZibraLiquidBridge::IsPaidVersion = NULL;
		ZibraLiquidBridge::RegisterParticlesBuffers = NULL;
		ZibraLiquidBridge::RegisterRenderResources = NULL;
		ZibraLiquidBridge::RegisterManipulators = NULL;
		ZibraLiquidBridge::SetRenderParameters = NULL;
		ZibraLiquidBridge::RegisterSolverBuffers = NULL;
		ZibraLiquidBridge::Draw = NULL;
		ZibraLiquidBridge::ClearSDFAndIDGrid = NULL;
		ZibraLiquidBridge::StepPhysics = NULL;
		ZibraLiquidBridge::UpdateLiquidParameters = NULL;
		ZibraLiquidBridge::SetCameraParameters = NULL;
		ZibraLiquidBridge::SaveAPIState = NULL;
		ZibraLiquidBridge::RestoreAPIState = NULL;
		ZibraLiquidBridge::ReleaseResources = NULL;
		ZibraLiquidBridge::InitializeGpuReadback = NULL;
		ZibraLiquidBridge::UpdateManipulatorParameters = NULL;
		ZibraLiquidBridge::GPUReadbackGetData = NULL;
		ZibraLiquidBridge::UpdateMeshRenderGlobalParameters = NULL;
		ZibraLiquidBridge::SetDebugLogWrapperPointer = NULL;
		ZibraLiquidBridge::InitializeGraphicsPipeline = NULL;
#endif // WITH_ZIBRA_AI

		FPlatformProcess::FreeDllHandle(Zibra_DLLHandle);
		Zibra_DLLHandle = NULL;
	}
}

PRAGMA_DISABLE_OPTIMIZATION
bool FZibraAILiquidRuntimeModule::ImportDLLMethods()
{
#if WITH_ZIBRA_AI

#define ZIB_ADD_FUNCTION(name) TPair<void**, const TCHAR*>((void**)&ZibraLiquidBridge::name, TEXT(#name))

	static const TPair <void**, const TCHAR*> functionsToLoad[] =
	{
		ZIB_ADD_FUNCTION(CreateLiquidInstance),
		ZIB_ADD_FUNCTION(IsPaidVersion),
		ZIB_ADD_FUNCTION(RegisterParticlesBuffers),
		ZIB_ADD_FUNCTION(RegisterRenderResources),
		ZIB_ADD_FUNCTION(RegisterManipulators),
		ZIB_ADD_FUNCTION(SetRenderParameters),
		ZIB_ADD_FUNCTION(RegisterSolverBuffers),
		ZIB_ADD_FUNCTION(UnrealPluginLoad),
		ZIB_ADD_FUNCTION(Draw),
		ZIB_ADD_FUNCTION(ClearSDFAndIDGrid),
		ZIB_ADD_FUNCTION(StepPhysics),
		ZIB_ADD_FUNCTION(UpdateReadback),
		ZIB_ADD_FUNCTION(UpdateLiquidParameters),
		ZIB_ADD_FUNCTION(SetCameraParameters),
		ZIB_ADD_FUNCTION(SaveAPIState),
		ZIB_ADD_FUNCTION(RestoreAPIState),
		ZIB_ADD_FUNCTION(ReleaseResources),
		ZIB_ADD_FUNCTION(InitializeGpuReadback),
		ZIB_ADD_FUNCTION(UpdateManipulatorParameters),
		ZIB_ADD_FUNCTION(GPUReadbackGetData),
		ZIB_ADD_FUNCTION(UpdateMeshRenderGlobalParameters),
		ZIB_ADD_FUNCTION(SetDebugLogWrapperPointer),
		ZIB_ADD_FUNCTION(InitializeGraphicsPipeline),
		ZIB_ADD_FUNCTION(GetCurrentAffineBufferIndex),
		ZIB_ADD_FUNCTION(GarbageCollect)
	};

#undef ZIB_ADD_FUNCTION

	for (auto& func : functionsToLoad)
	{
		*func.Key = FPlatformProcess::GetDllExport(Zibra_DLLHandle, func.Value);
		if (*func.Key == nullptr)
		{
			UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("Load function %s"), func.Value);
			return false;
		}
	}

	BridgeStructs::LoggerSettings settings;
	settings.PFNCallback = &BridgeStructs::DebugLogCallback;
	settings.LogLevel = BridgeStructs::LogLevel::Error;
	ZibraLiquidBridge::SetDebugLogWrapperPointer(&settings);

	return true;
#endif //WITH_ZIBRA_AI
	return false;
}
PRAGMA_ENABLE_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZibraAILiquidRuntimeModule, ZibraAILiquidRuntime)