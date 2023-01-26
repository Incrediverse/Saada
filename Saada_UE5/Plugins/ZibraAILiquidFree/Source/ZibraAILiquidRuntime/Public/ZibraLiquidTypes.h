// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZibraLiquidDefines.h"
#include "PixelFormat.h"
#include "ZibraLiquidTypes.generated.h"


struct FTextureUploadData
{
	void* data;
	int dataSize;
	int rowPitch;
	int dimensionX;
	int dimensionY;
	int dimensionZ;
};

enum class ETextureFormat
{
	ETextureFormat_None,
	ETextureFormat_R8G8B8A8_SNorm,
	ETextureFormat_R16G16B16A16_SFloat,
	ETextureFormat_R32G32B32A32_SFloat,
	ETextureFormat_R16_SFloat,
	ETextureFormat_R32_SFloat,
	ETextureFormat_R32G8X24_TYPELESS
};

struct FTextureBridge
{
	void* texture;
	ETextureFormat format;
};

//ZibraFluidNative
struct FCameraParams
{
	ZibraMatrix View;
	ZibraMatrix Projection;
	ZibraMatrix ProjectionInverse;
	ZibraMatrix ViewProjection;
	ZibraMatrix ViewProjectionInverse;
	ZibraMatrix EyeRayCameraCoeficients;
	ZibraVector3 WorldSpaceCameraPos;
	int32 CameraID;
	ZibraVector2 CameraResolution;
	float CameraParamsPadding1;
	float CameraParamsPadding2;
};

struct FRenderParams
{
	float BlurRadius;
	float Diameter;
	float NeuralSamplingDistance;
	float SDFDebug;

	int32 RenderingMode;
	int32 VertexOptimizationIterations;
	int32 MeshOptimizationIterations;
	int32 JFAIterations;

	float DualContourIsoValue;
	float MeshOptimizationStep;
	float CameraDensity;
	int32 RenderParamsPadding1;

	int MaxVertexBufferSize;
	int MaxIndexBufferSize;
	FIntPoint RenderParamsPadding2;
};

struct FSimulationParams
{
	ZibraVector3 GridSize;
	int32 ParticleCount;

	ZibraVector3 ContainerScale;
	int32 NodeCount;

	ZibraVector3 ContainerPos;
	float TimeStep;

	ZibraVector3 Gravity;
	int32 SimulationFrame;

	float DensityBlurRadius;
	float LiquidIsosurfaceThreshold;
	float VertexOptimizationStep;
	float AffineAmount;

	ZibraVector3 ParticleTranslation;
	float VelocityLimit;

	float LiquidStiffness;
	float RestDensity;
	float SurfaceTension;
	float AffineDivergenceDecay;

	float MinimumVelocity;
	float BlurNormalizationConstant;
	int32 MaxParticleCount;
	int32 VisualizeSDF;
};

struct FManipulatorParams
{
public:
	ZibraMatrix Transform;
	ZibraMatrix PreviousTransform;
	ZibraMatrix ToObjectSpace;
	ZibraVector3 Position;
	int32 ConstDataBegin;
	ZibraVector3 Scale;
	float ManipulatorParamPadding;
	ZibraVector4 Rotation;
	ZibraVector3 BBoxMin;
	int32 Type;
	ZibraVector3 BBoxMax;
	int32 Enabled;
	ZibraVector4 AdditionalData;
};

struct FRegisterParticlesBuffersBridgeParams
{
public:
	void* PositionMass;
	void* AffineVelocity0;
	void* AffineVelocity1;
	void* PositionRadius;
	void* ParticleNumber;
};

struct FInitializeGPUReadbackParams
{
	uint32 ReadbackBufferSize;
	int32 MaxInFlightFrames;
};

union FUEInterface
{
	struct UED3D12Interace 
	{
		void* Device;
		void* Queue;
	} D3D12;
	struct UED3D11Interface
	{
		void* Device;
	} D3D11;
};

struct FRegisterManipulatorsBridgeParams
{
	int32 ManipulatorNum;
	void* ManipulatorBufferDynamic;
	void* ManipulatorBufferConst;
	void* ManipulatorBufferStatistics;
	void* ManipulatorParams;
	int32 ConstDataSize;
	void* ConstManipulatorData;
	void* ManipIndices;
	FTextureBridge EmbeddingsTexture;
	FTextureBridge SDFGridTexture;
	FTextureUploadData EmbeddigsData;
	FTextureUploadData SDFGridData;
};

struct FRegisterSolverBuffersBridgeParams
{
	void* SimulationParams;
	void* PositionMassCopy;
	void* GridData;
	void* IndexGrid;
	void* GridBlur0;
	void* GridBlur1;
	void* GridNormal;
	void* GridSDF;
	void* NodeParticlePairs0;
	void* NodeParticlePairs1;
	void* RadixGroupData1;
	void* RadixGroupData2;
	void* RadixGroupData3;
	void* Counters;
	void* VertexIDGrid;
	void* VertexBuffer0;
	void* VertexBuffer1;
	void* QuadBuffer;
	void* TransferDataBuffer;
	void* MeshRenderIndexBuffer;
	void* NativeMeshVertexBuffer;
	void* NativeMeshIndexBuffer;
	void* VertexData;
	FTextureBridge GridNormals;
	FTextureBridge GridDensity;
};

struct FRegisterRenderResourcesBridgeParams
{
	void* AtomicGrid;
	void* JFA0;
	void* JFA1;
	FTextureBridge Depth;
	FTextureBridge Color0;
	FTextureBridge Color1;
	FTextureBridge SDFRender;
};

enum ELogLevel
{
	ZibraLogLevel_Verbose = 0 UMETA(DisplayName = "Verbose"),
	ZibraLogLevel_Info = 1 UMETA(DisplayName = "Info"),
	ZibraLogLevel_Performance = 2 UMETA(DisplayName = "Performance"),
	ZibraLogLevel_Warning = 3 UMETA(DisplayName = "Warning"),
	ZibraLogLevel_Error = 4 UMETA(DisplayName = "Error"),
};

struct FDebugMessage
{
	const char* text = nullptr;
	ELogLevel level = ELogLevel::ZibraLogLevel_Info;
};

struct FLoggerSettings
{
	void* PFNCallback;
	ELogLevel LogLevel;
};

static ETextureFormat ToBridgeTextureFormat(EPixelFormat format)
{
	switch (format)
	{
	case EPixelFormat::PF_FloatRGBA:
		return ETextureFormat::ETextureFormat_R16G16B16A16_SFloat;
	case EPixelFormat::PF_A32B32G32R32F:
		return ETextureFormat::ETextureFormat_R32G32B32A32_SFloat;
	case EPixelFormat::PF_R16F:
		return ETextureFormat::ETextureFormat_R16_SFloat;
	case EPixelFormat::PF_R32_FLOAT:
		return ETextureFormat::ETextureFormat_R32_SFloat;
	case EPixelFormat::PF_DepthStencil:
		return ETextureFormat::ETextureFormat_R32G8X24_TYPELESS;
	default:
		return ETextureFormat::ETextureFormat_None;
	}
}
struct FMeshRenderGlobalParams
{
	ZibraVector3 ContainerPosition;
	float LiquidIOR;

	float RayMarchIsoSurface;
	int UnderwaterRender;
	float RayMarchStepSize;
	float RayMarchStepFactor;

	int RayMarchMaxSteps;
	int TwoBouncesEnabled;
	FIntPoint RayMarchResolution;
};

//end ZibraFluidNative

//struct FCameraResources
//{
//	FTexture2DRHIRef background;
//	bool bIsDirty = true;
//};

struct FManipulatorIndices
{
	int32 EmitterIndex;
	int32 EmitterIndexEnd;

	int32 VoidIndex;
	int32 VoidIndexEnd;

	int32 ForceFieldIndex;
	int32 ForceFieldIndexEnd;

	int32 AnalyticColliderIndex;
	int32 AnalyticColliderIndexEnd;

	int32 ZibraNeuralColliderIndex;
	int32 ZibraNeuralColliderIndexEnd;

	int32 DetectorIndex;
	int32 DetectorIndexEnd;

	int32 PortalIndex;
	int32 PortalIndexEnd;
	FVector2D ManipulatorIndicesPadding;
};

USTRUCT()
struct FZibraLiquidSolverParams
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, meta = (Tooltip = "The strength and direction of the gravity", Category = "Zibra Liquid Solver Parameters"))
	FVector Gravity = FVector(0.0f, 0.0f, -9.81f);

	UPROPERTY(EditAnywhere, meta = (Tooltip = "The stiffness of the liquid, recommended 0.1f", ClampMin = "0.0", Category = "Zibra Liquid Solver Parameters"))
	float FluidStiffness = 0.1f;

	UPROPERTY( meta = (Tooltip = "The sharpness of the stiffness, recommended 4.0f", ClampMin = "1.0", ClampMax = "8.0"))
	float FluidStiffnessPower = 3.0f;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Resting density of particles. measured in particles/cell. This option directly affects volume of each particle. Higher values correspond to less volume, but higher quality simulation.", ClampMin = "0.1", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	float ParticleDensity = 2.0f;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "The velocity limit of the particles", ClampMin = "0.0", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	float MaximumVelocity = 3.0f;

	//UPROPERTY(EditAnywhere, meta = (Tooltip = "Minimum velocity the particles can have, non-zero values make an infinite flow. Normally this should be 0", ClampMin = "0.0", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	float MinimumVelocity = 0.0f;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Viscosity of the liquid", ClampMin = "0.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	float Viscosity = 0.392;
	
	//UPROPERTY(EditAnywhere, meta = (Tooltip = "Surface tension, you can also set it to negative to get a spagettification effect", ClampMin = "-1.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	float SurfaceTension = 0.0f;

	//UPROPERTY(EditAnywhere, meta = (Tooltip = "The strenght of the force acting on rigid bodies, exponential scale, from exp(-4) to exp(4)", ClampMin = "-1.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	float ForceInteractionStrength = 0.0f;
};

USTRUCT()
struct FZibraLiquidMaterialParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The color of the liquid body"))
	FColor Color = FColor(39, 187, 207, 1.0f);

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The color of the liquid reflection mod (A - fresnel mod)"))
	FColor ReflectionColor = FColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The emissive color of the liquid. Normally black for most liquids."))
	FColor EmissiveColor = FColor(0, 0, 0, 0);

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The multiplier for the brightness of the emissive color of the liquid."))
	float EmmisiveMultiplier = 1000;

	float NeuralSamplingDistance = 1.0f;
	
	float SDFDebug = 0.0f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The Roughness of the liquid body", ClampMin = "0.0", ClampMax = "1.0"))
	float Roughness = 0.0f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The metalness of the surface", ClampMin = "0.0", ClampMax = "1.0"))
	float Metalness = 0.045f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light being scattered by the liquid volume. Visually adds a fog to the fluid volume. Maximum value makes the liquid opaque.", ClampMin = "0.0", ClampMax = "30.0"))
	float ScatteringAmount = 0;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume.", ClampMin = "0.0", ClampMax = "1.0"))
	float Opacity = 0.0;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume. Visually darkens all colors except to the selected liquid color.", ClampMin = "0.0", ClampMax = "30.0"))
	float AbsorptionAmount = 5.2f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The index of refraction", ClampMin = "1.0", ClampMax = "3.0"))
	float IndexOfRefraction = 1.333f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The radius of the blur of the liquid density on the simulation grid. Controls the smoothness of the normals.", ClampMin = "0.01", ClampMax = "4.0"))
	float FluidSurfaceBlur = 1.5f;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Particle rendering scale compared to the cell size", ClampMin = "0.0", ClampMax = "4.0"))
	float ParticleScale = 2;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", ClampMin = "0.0", ClampMax = "2.0"))
	float FoamIntensity = 0.8f;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Foam appearance threshold.", ClampMin = "0.0", ClampMax = "4.0"))
	float FoamAmount = 1.0f;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Blur radius of particle normals and depth.", ClampMin = "0.0001", ClampMax = "1.0"))
	float BlurRadius = 0.2f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Toooltip = "The brightness of the reflection", ClampMin = "0.0", ClampMax = "1.0"))
	float SpecularIntensity = 1;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The brightness of the borders of the reflection", ClampMin = "0.0", ClampMax = "1.0"))
	float FresnelStrength = 0.5;

	float BilateralWeight = 2.5f;

	int ObjectVersion = 1;
};


struct FColliderParams
{
	FVector4 Rotation;
	FVector BBoxMin;
	int32 SDFType;
	FVector BBoxMax;
	int32 iteration;
	FVector Position;
	int32 VoxelCount;
	FVector Scale;
	int32 OpType;
	int32 Elements;
	int32 CurrentID;
	int32 colliderIndex;
};

USTRUCT()
struct FObjectTransform
{
	GENERATED_BODY()

		UPROPERTY()
		FString Q;

	UPROPERTY()
		FString T;

	UPROPERTY()
		FString S;
};


UENUM()
enum ERenderingMode
{
	ERenderingMode_ParticleRender UMETA(DisplayName = "Particle Render"),
	ERenderingMode_MeshRender UMETA(DisplayName = "Mesh Render")
};

//Advanced Parameters
UENUM()
enum ELiquidRefractionQuality
{
	ELiquidRefractionQuality_PerVertexRender UMETA(DisplayName = "Per Vertex Render"),
	ELiquidRefractionQuality_PerPixelRender UMETA(DisplayName = "Per Pixel Render")
};
UENUM()
enum ERayMarchingBounces
{
	ERayMarchingBounces_SingleBounce UMETA(DisplayName = "Single Bounce"),
	ERayMarchingBounces_TwoBounces UMETA(DisplayName = "Two Bounces")
};

USTRUCT()
struct FZibraLiquidAdvancedRenderParameters
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "How much to downscale the ray march resolution. Can improve performance.", ClampMin = "0.0", ClampMax = "1.0"))
	float RayMarchingResolutionDownscale = 1.0f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the quality of the liquid depth"))
	TEnumAsByte<ELiquidRefractionQuality> RefractionQuality = ELiquidRefractionQuality::ELiquidRefractionQuality_PerVertexRender;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of bounces of the refraction ray, to see the liquid behing itself you need 2 bounces"))
	TEnumAsByte <ERayMarchingBounces> RefractionBounces = ERayMarchingBounces::ERayMarchingBounces_SingleBounce;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Enable underwater rendering. Disable it if you don't need it, since it's a bit slower."))
	bool bUnderwaterRender = false;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The maximum fraction of triangles the mesh can have (below 1.0f there is a chance of rendering only part of the mesh). Has a large effect on VRAM and performance.", ClampMin = "0.0", ClampMax = "1.0"))
	float MaxLiquidMeshSize = 1.0f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Additional JFA Iterations", Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of additional sphere render iterations, controls how large spheres can get, has a large impact on performance", ClampMin = "0", ClampMax = "8"))
	int AdditionalJFAIterations = 0;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of iterations that move the mesh vertex to the liquid isosurface", ClampMin = "0", ClampMax = "20"))
	int VertexOptimizationIterations = 5;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of smoothing iterations for the mesh", ClampMin = "0", ClampMax = "8"))
	int MeshOptimizationIterations = 2;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh.", ClampMin = "0.0", ClampMax = "2.0"))
	float VertexOptimizationStep = 0.82f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh.", ClampMin = "0.0", ClampMax = "1.0"))
	float MeshOptimizationStep = 0.91f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isovalue at which the mesh vertices are generated", ClampMin = "0.0", ClampMax = "2.0"))
	float DualContourIsoSurfaceLevel = 0.025f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the position of the fluid surface. Lower values result in thicker surface.", ClampMin = "0.01", ClampMax = "2.0"))
	float IsoSurfaceLevel = 0.36f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isosufrace level for the ray marching. Should be about 1-1/2 of the liquid density.", ClampMin = "0.0", ClampMax = "5.0"))
	float RayMarchIsoSurface = 0.65f;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Maximum number of steps the ray can go, has a large effect on the performance", ClampMin = "4", ClampMax = "128"))
	int RayMarchMaxSteps = 128;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Step size of the ray marching, controls accuracy, also has a large effect on performance", ClampMin = "0.0", ClampMax = "1.0"))
	float RayMarchStepSize = 0.1;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Varies the ray marching step size, in some cases might improve performace by slightly reducing ray marching quality", ClampMin = "1.0", ClampMax = "10.0"))
	float RayMarchStepFactor = 4.0f;
};

struct FTexture3DSetupData
{
	TArray<uint8> SDFGrid;
	TArray<FColor> Embeddings;
};


UENUM()
enum EZibraLicenseKeyStatus
{
	EZibraLicenseKeyStatus_OK UMETA(DisplayName = "OK"),
	EZibraLicenseKeyStatus_KeyValidationInProgress UMETA(DisplayName = "Key Validation In Progress"),
	EZibraLicenseKeyStatus_NetworkError UMETA(DisplayName = "Network Error"),
	EZibraLicenseKeyStatus_NotRegistered UMETA(DisplayName = "Not Registered"),
	EZibraLicenseKeyStatus_NotInitialized UMETA(DisplayName = "Not Initialized"),
};

USTRUCT()
struct FLicenseKeyResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString api_key;

};