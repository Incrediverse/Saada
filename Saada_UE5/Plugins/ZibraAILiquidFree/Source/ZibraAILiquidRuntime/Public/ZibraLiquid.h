// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraLiquidTypes.h"
#include "ZibraLiquidBridge.h"
#include "ZibraManipulatorManager.h"
#include "ZibraManipulator.h"
#include "ZibraSDFCollider.h"
#include "ZibraLiquidProxy.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstanceConstant.h"
#include "RHI.h"
#include "Materials/Material.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "PhysicsPublic.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "Shader.h"
#include "ShaderCompilerCore.h"
#include "GlobalShader.h"
#include "MeshBatch.h"
#include "MaterialShaderType.h"
#include "MaterialShader.h"
#include "MeshPassProcessor.h"
#include "MeshMaterialShader.h"
#include "MeshPassProcessor.inl"
#include "Engine/TextureRenderTarget.h"
#include "Engine/TextureRenderTargetVolume.h"
#include "Engine/Texture2DDynamic.h"
#include "ZibraTexture2DDynamic.h"
#include "Engine/TextureCube.h"

#include "ZibraLiquid.generated.h"

class UStaticMeshComponent;


UCLASS(Blueprintable)//, hideCategories = (Rendering, Replication, Input, Actor, Base, Shape, OverlapCapsule, Physics, Cooking))
class ZIBRAAILIQUIDRUNTIME_API AZibraLiquid : public AActor
{
    GENERATED_BODY()

public:
    AZibraLiquid(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float inDeltaTime) override;
	virtual void BeginDestroy() override;
	virtual bool IsReadyForFinishDestroy() override;
	virtual void FinishDestroy() override;
	void EditorTick(float inDeltaTime);
	virtual bool ShouldTickIfViewportsOnly() const override;

    void SetContainerPosition(const FVector& newPosition) { ContainerPosition = newPosition; }
	FVector GetContainerPosition();
	void RenderCallBack(FPostOpaqueRenderParameters& Parameters);
	// Can be called in normal tick or in substep tick
	static void SimulationCallBack(ZibraLiquidProxy* Proxy, FRHICommandListImmediate& RHICmdList, int IterationsPerFrameCopy, FSimulationParams FluidParametersCopy, const TArray<FManipulatorParams>& ManipulatorParamsCopy, int ManipulatorManagerElements);
	void UpdateReadback();
	
	
	float GetCellSize();
	///UI Zibra Liquid
	UPROPERTY(EditAnywhere, SimpleDisplay, BlueprintReadWrite, DisplayName = "Container Size", Category = "Zibra Liquid")
	FVector ContainerSize = { 10.0f, 10.0f, 10.0f };

	FVector ContainerSizeScaled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "SDF Colliders", Category = "Zibra Liquid", meta = (AllowedClasses = "UZibraManipulator"))
	TArray<AActor*> ZibraSDFColliderActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Manipulators", Category = "Zibra Liquid", meta = (AllowedClasses = "UZibraManipulator"))
	TArray<AActor*> ManipulatorActors;

	UFUNCTION(CallInEditor, Category = "Zibra Liquid", meta = (DisplayName = "Add All Colliders"))
	void AddAllColliders();

	UFUNCTION(CallInEditor, Category = "Zibra Liquid", meta = (DisplayName = "Add All Manipulators"))
	void AddAllManipulators();

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0001", ClampMax = "1.0", DisplayName = "Maximum Allowed Timestep", Tooltip = "The maximum allowed simulation timestep", Category = "Zibra Liquid"))
	float TimeStepMax = 0.5f;
	
	//UPROPERTY(EditAnywhere, meta = (ClampMin = "2", ClampMax = "16", DisplayName = "Maximum Frames In Flight", Tooltip = "Fallback max frame latency. Used when it isn't possible to retrieve Unity's max frame latency.", Category = "Zibra Liquid"))
	// The more frames in flight we have, the higher the latency of force interaction is
	// And so force interaction is less stable
	uint32 maxFramesInFlight = 6;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "100.0", DisplayName = "Simulation Speed", Tooltip = "The speed of the simulation, how many simulation time units per second", Category = "Zibra Liquid"))
	float SimTimePerSec = 40.0f;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, SkipSerialization, DisplayName = "Current particle count", Category = "Zibra Liquid")
	int32 ActiveParticleCount = 0;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "10", DisplayName = "Iterations Per Frame", Tooltip = "The number of solver iterations per frame, in most cases one iteration is sufficient", Category = "Zibra Liquid"))
	int IterationsPerFrame = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1024", ClampMax = "2097152", DisplayName = "Max Particle Count", Tooltip = "Clamps maximum particles count", Category = "Zibra Liquid"))
	int MaxNumParticles = 262144;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "16", Tooltip = "Sets the resolution of the largest sid of the grids container equal to this value", Category = "Zibra Liquid"))
	int GridResolution = 128;
	
	//UPROPERTY(VisibleAnywhere, meta = (DisplayName = "Effective Grid Resolution", Tooltip = "Sets the resolution of the largest sid of the grids container equal to this value", Category = "Zibra Liquid"))
	FVector EffectiveGridResolution;
	
	UPROPERTY(EditAnywhere, DisplayName="Run Simulaton", meta = (Tooltip = "Controls if simulation is active", Category = "Zibra Liquid"))
	bool bRunSimulation = true;

	//UPROPERTY(EditAnywhere, DisplayName = "Run Rendering", meta = (Tooltip = "Controls if rendering is active", Category = "Zibra Liquid"))
	bool bRunRendering = true;

	//UPROPERTY(EditAnywhere, DisplayName = "Use Fixed Timestep", meta = (Category = "Zibra Liquid"))
	bool bUseFixedTimestep = false;

	UPROPERTY(EditAnywhere, DisplayName = "Visualize Scene SDF", meta = (Category = "Zibra Liquid"))
	bool bVisualizeSceneSDF = false;

	UPROPERTY(VisibleAnywhere, DisplayName = "Scene SDF Visualization", meta = (Category = "Zibra Liquid", Tooltip = "Click on this texture to open the visualization"))
	UTextureRenderTarget2D* SDFRenderRT;

	//UPROPERTY(EditAnywhere, DisplayName = "Reflection Cubemap", meta = (UseComponentPicker, Category = "Zibra Liquid"))
	UTextureCube* ReflectionCubemap;

	//UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid"))
	bool bEnableDownscale = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Particle Count Footprint", Category = "Zibra Liquid")
	FString ParticleCountFootprintDisplay;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Colliders Footprint", Category = "Zibra Liquid")
	FString CollidersDisplay; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Grid Size Footprint", Category = "Zibra Liquid")
	FString GridSizeDisplay;


	// bool bIsSimulatingInBackground;
	// 

	//-- END UI Zibra Liquid

	TArray<UZibraSDFCollider*> ZibraSDFColliders;
	TArray<UZibraManipulator*> manipulators;

	//API
	//Zibra Liquid

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	FVector GetContainerSize();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void SetContainerSize(FVector NewValue);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void AddZibraSDFCollider(UZibraSDFCollider* NewCollider);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void RemoveZibraSDFCollider(UZibraSDFCollider* NewCollider);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void AddManipulator(UZibraManipulator* NewManipulator);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void RemoveManipulator(UZibraManipulator* Manipulator);
	
	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The maximum allowed simulation timestep", Category = "Zibra Liquid"))
	float GetTimeStepMax();

	UFUNCTION(BlueprintCallable, meta = (ClampMin = "0.0001", ClampMax = "1.0", Tooltip = "The maximum allowed simulation timestep", Category = "Zibra Liquid"))
	void SetTimeStepMax(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The speed of the simulation, how many simulation time units per second", Category = "Zibra Liquid"))
	float GetSimulationSpeed();

	UFUNCTION(BlueprintCallable, meta = (ClampMin = "1.0", ClampMax = "100.0", Tooltip = "The speed of the simulation, how many simulation time units per second", Category = "Zibra Liquid"))
	void SetSimulationSpeed(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The count of active particles", Category = "Zibra Liquid"))
	int32 GetActiveParticleCount();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The number of solver iterations per frame, in most cases one iteration is sufficient", Category = "Zibra Liquid"))
	int GetIterationsPerFrame();

	UFUNCTION(BlueprintCallable, meta = (ClampMin = "1", ClampMax = "10", Tooltip = "The number of solver iterations per frame, in most cases one iteration is sufficient", Category = "Zibra Liquid"))
	void SetIterationsPerFrame(int NewValue);

	UFUNCTION(BlueprintCallable, meta = ( Tooltip = "Clamps maximum particles count", Category = "Zibra Liquid"))
	float GetMaxNumParticles();

	UFUNCTION(BlueprintCallable, meta = (ClampMin = "1024", ClampMax = "10000000", Tooltip = "Clamps maximum particles count", Category = "Zibra Liquid"))
	void SetMaxNumParticles(float NewValue);

	UFUNCTION(BlueprintCallable, meta = ( Tooltip = "Sets the resolution of the largest sid of the grids container equal to this value", Category = "Zibra Liquid"))
	void SetGridResolution(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (ClampMin = "16", Tooltip = "Sets the resolution of the largest sid of the grids container equal to this value", Category = "Zibra Liquid"))
	int GetGridResolution();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	bool SimulationRunning();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void RunSimulation();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void StopSimulation();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	bool SDFVisualizationRunning();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void RunSDFVisualization();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	void StopSDFVisualization();

	//UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	ULONG GetParticleCountFootprint();

	//UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	ULONG GetCollidersFootprint();

	//UFUNCTION(BlueprintCallable, Category = "Zibra Liquid")
	ULONG GetGridFootprint();
	
	//end Zibra Liquid

	//Zibra Liquid Solver Parameters

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The strength and direction of the gravity", Category = "Zibra Liquid Solver Parameters"))
	FVector GetGravity();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The strength and direction of the gravity", Category = "Zibra Liquid Solver Parameters"))
	void SetGravity(FVector NewValue);
	
	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The stiffness of the liquid, recommended 0.1f", Category = "Zibra Liquid Solver Parameters"))
	float GetFluidStiffness();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The stiffness of the liquid, recommended 0.1f", ClampMin = "0.0", Category = "Zibra Liquid Solver Parameters"))
	void SetFluidStiffness(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Resting density of particles. measured in particles/cell. This option directly affects volume of each particle. Higher values correspond to less volume, but higher quality simulation.", Category = "Zibra Liquid Solver Parameters"))
	float GetParticleDensity();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Resting density of particles. measured in particles/cell. This option directly affects volume of each particle. Higher values correspond to less volume, but higher quality simulation.", ClampMin = "0.1", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	void SetParticleDensity(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The velocity limit of the particles", Category = "Zibra Liquid Solver Parameters"))
	float GetMaximumVelocity();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The velocity limit of the particles", ClampMin = "0.0", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	void SetMaximumVelocity(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Minimum velocity the particles can have, non-zero values make an infinite flow. Normally this should be 0", Category = "Zibra Liquid Solver Parameters"))
	float GetMinimumVelocity();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Minimum velocity the particles can have, non-zero values make an infinite flow. Normally this should be 0", ClampMin = "0.0", ClampMax = "10.0", Category = "Zibra Liquid Solver Parameters"))
	void SetMinimumVelocity(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Viscosity of the liquid", Category = "Zibra Liquid Solver Parameters"))
	float GetViscosity();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Viscosity of the liquid", ClampMin = "0.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	void SetViscosity(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Surface tension, you can also set it to negative to get a spagettification effect", Category = "Zibra Liquid Solver Parameters"))
	float GetSurfaceTension();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "Surface tension, you can also set it to negative to get a spagettification effect", ClampMin = "-1.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	void SetSurfaceTension(float NewValue);
	
	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The strenght of the force acting on rigid bodies, exponential scale, from exp(-4) to exp(4)", Category = "Zibra Liquid Solver Parameters"))
	float GetForceInteractionStrength();

	UFUNCTION(BlueprintCallable, meta = (Tooltip = "The strenght of the force acting on rigid bodies, exponential scale, from exp(-4) to exp(4)", ClampMin = "-1.0", ClampMax = "1.0", Category = "Zibra Liquid Solver Parameters"))
	void SetForceInteractionStrength(float NewValue);

	//end Zibra Liquid Simulation Parameters

	//Zibra Liquid Material Parameters

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The color of the liquid body"))
	FColor GetColor();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The color of the liquid body"))
	void SetColor(FColor NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The emissive color of the liquid. Normally black for most liquids."))
	FColor GetEmissiveColor();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The emissive color of the liquid. Normally black for most liquids."))
	void SetEmissiveColor(FColor NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The Roughness of the liquid body"))
	float GetRoughness();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The Roughness of the liquid body", ClampMin = "0.0", ClampMax = "1.0"))
	void SetRoughness(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The metalness of the surface"))
	float GetMetalness();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The metalness of the surface", ClampMin = "0.0", ClampMax = "1.0"))
	void SetMetalness(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light being scattered by the liquid volume. Visually adds a fog to the fluid volume. Maximum value makes the liquid opaque."))
	float GetScatteringAmount();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light being scattered by the liquid volume. Visually adds a fog to the fluid volume. Maximum value makes the liquid opaque.", ClampMin = "0.0", ClampMax = "30.0"))
	void SetScatteringAmount(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume."))
	float GetOpacity();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume.", ClampMin = "0.0", ClampMax = "1.0"))
	void SetOpacity(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume. Visually darkens all colors except to the selected liquid color."))
	float GetAbsorptionAmount();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The amount of light absorbed in the liquid volume. Visually darkens all colors except to the selected liquid color.", ClampMin = "0.0", ClampMax = "30.0"))
	void SetAbsorptionAmount(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The index of refraction"))
	float GetIndexOfRefraction();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The index of refraction", ClampMin = "1.0", ClampMax = "3.0"))
	void SetIndexOfRefraction(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The radius of the blur of the liquid density on the simulation grid. Controls the smoothness of the normals."))
	float GetFluidSurfaceBlur();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "The radius of the blur of the liquid density on the simulation grid. Controls the smoothness of the normals.", ClampMin = "0.01", ClampMax = "4.0"))
	void SetFluidSurfaceBlur(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Particle rendering scale compared to the cell size"))
	float GetParticleScale();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Particle rendering scale compared to the cell size", ClampMin = "0.0", ClampMax = "4.0"))
	void SetParticleScale(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Blur radius of particle normals and depth."))
	float GetBlurRadius();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Material Parameters", Tooltip = "Blur radius of particle normals and depth.", ClampMin = "0.0001", ClampMax = "1.0"))
	void SetBlurRadius(float NewValue);

	//end Zibra Liquid Material Parameters
		
	//Zibra Liquid Advanced Render Parameters

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "How much to downscale the ray march resolution. Can improve performance.", ClampMin = "0.0", ClampMax = "1.0"))
	void SetRayMarchingResolutionDownscale(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "How much to downscale the ray march resolution. Can improve performance."))
	float GetRayMarchingResolutionDownscale();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the quality of the liquid depth"))
	ELiquidRefractionQuality	GetRefractionQuality();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the quality of the liquid depth"))
	void SetRefractionQuality(ELiquidRefractionQuality NewValue);
	
	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of bounces of the refraction ray, to see the liquid behing itself you need 2 bounces"))
	ERayMarchingBounces	GetRefractionBounces();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of bounces of the refraction ray, to see the liquid behing itself you need 2 bounces"))
	void SetRefractionBounces(ERayMarchingBounces NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Enable underwater rendering. Disable it if you don't need it, since it's a bit slower."))
	bool UnderwaterRenderEnabled();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Enable underwater rendering. Disable it if you don't need it, since it's a bit slower."))
	void SetUnderwaterRender(bool NewValue);


	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The maximum fraction of triangles the mesh can have (below 1.0f there is a chance of rendering only part of the mesh). Has a large effect on VRAM and performance."))
	float GetMaxLiquidMeshSize();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The maximum fraction of triangles the mesh can have (below 1.0f there is a chance of rendering only part of the mesh). Has a large effect on VRAM and performance.", ClampMin = "0.0", ClampMax = "1.0"))
	void SetMaxLiquidMeshSize(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of additional sphere render iterations, controls how large spheres can get, has a large impact on performance"))
	int GetAdditionalJFAIterations();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of additional sphere render iterations, controls how large spheres can get, has a large impact on performance", ClampMin = "0", ClampMax = "8"))
	void SetAdditionalJFAIterations(int NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of iterations that move the mesh vertex to the liquid isosurface"))
	int32 GetVertexOptimizationIterations();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of iterations that move the mesh vertex to the liquid isosurface", ClampMin = "0", ClampMax = "20"))
	void SetVertexOptimizationIterations(int32 NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of smoothing iterations for the mesh"))
	int GetMeshOptimizationIterations();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Number of smoothing iterations for the mesh", ClampMin = "0", ClampMax = "8"))
	void SetMeshOptimizationIterations(int NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh."))
	float GetVertexOptimizationStep();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh.", ClampMin = "0.0", ClampMax = "2.0"))
	void SetVertexOptimizationStep(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh."))
	float GetMeshOptimizationStep();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "This parameter moves liquid mesh vertices to be closer to the actual liquid surface. It should be manually fine tuned until you get a smooth mesh.", ClampMin = "0.0", ClampMax = "1.0"))
	void SetMeshOptimizationStep(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isovalue at which the mesh vertices are generated"))
	float GetDualContourIsoSurfaceLevel();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isovalue at which the mesh vertices are generated", ClampMin = "0.0", ClampMax = "2.0"))
	void SetDualContourIsoSurfaceLevel(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the position of the fluid surface. Lower values result in thicker surface."))
	float GetIsoSurfaceLevel();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Controls the position of the fluid surface. Lower values result in thicker surface.", ClampMin = "0.01", ClampMax = "2.0"))
	void SetIsoSurfaceLevel(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isosufrace level for the ray marching. Should be about 1-1/2 of the liquid density."))
	float GetRayMarchIsoSurface();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "The isosufrace level for the ray marching. Should be about 1-1/2 of the liquid density.", ClampMin = "0.0", ClampMax = "5.0"))
	void SetRayMarchIsoSurface(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Maximum number of steps the ray can go, has a large effect on the performance"))
	int GetRayMarchMaxSteps();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Maximum number of steps the ray can go, has a large effect on the performance", ClampMin = "4", ClampMax = "128"))
	void SetRayMarchMaxSteps(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Step size of the ray marching, controls accuracy, also has a large effect on performance"))
	float GetRayMarchStepSize();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Step size of the ray marching, controls accuracy, also has a large effect on performance", ClampMin = "0.0", ClampMax = "1.0"))
	void SetRayMarchStepSize(float NewValue);

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Varies the ray marching step size, in some cases might improve performace by slightly reducing ray marching quality"))
	float GetRayMarchStepFactor();

	UFUNCTION(BlueprintCallable, meta = (Category = "Zibra Liquid Advanced Render Parameters", Tooltip = "Varies the ray marching step size, in some cases might improve performace by slightly reducing ray marching quality", ClampMin = "1.0", ClampMax = "10.0"))
	void SetRayMarchStepFactor(float NewValue);

	//end Zibra Liquid Advanced Render Parameters

	//END API

	UPROPERTY()
	UStaticMeshComponent* CubeComponent;

	UPROPERTY()
	bool bUseAsyncForceUpdate = true;
	
	UPROPERTY( meta = (ClampMin = "0.00001", Tooltip = "Main parameter that regulates the resolution of the simulation. Defines the size of the simulation grid cell in world length units"))
	float CellSize = 0.1f;

	UPROPERTY( meta = (ClampMin = "0.2", ClampMax = "0.99"))
	float DownscaleFactor = 0.5f;

	UPROPERTY(  meta = (ClampMin = "0.01", ClampMax = "16.0"))
	float EmitterDensity = 1.0f;
	
	UTextureRenderTargetVolume* RenderTargetGridNormals;
	UTextureRenderTargetVolume* RenderTargetGridDensity;

	UZibraTexture2DDynamic* Color0UT;
	UZibraTexture2DDynamic* Color1UT;
	UZibraTexture2DDynamic* DepthUT;

	UMaterialInstanceDynamic* FluidMaterial = nullptr;
	UMaterialInstanceConstant* TransparentFluid = nullptr;
	UMaterial* MeshRenderMaterial = nullptr;
	static TArray<AZibraLiquid*> AllLiquids;

	static void Init(ZibraLiquidProxy* Proxy, FRegisterSolverBuffersBridgeParams RegisterSolverBuffersBridgeParams, FRegisterParticlesBuffersBridgeParams RegisterParticlesBuffersParams, FRegisterManipulatorsBridgeParams RegisterManipulatorsBridgeParams, FInitializeGPUReadbackParams InitializeGPUReadbackParams);
	void UpdateRegisterParticlesBuffersParams();
	void UpdateRegisterSolverBuffersBridgeParams();

	// Call this when you want to hook onto the renderer and start drawing. The shader will be executed once per frame.
	void RegisterRenderThreadCallback();
	void RemoveDestroyedManipulator(UZibraManipulator* Manipulator);
	void RemoveDestroyedCollider(UZibraSDFCollider* Collider);

	int AvgFrameRate;
	float DeltaTime;
	float SmoothDeltaTime;

	bool bForceTextureUpdate = false;

	bool IsRenderingEnabled();
private:
	ZibraLiquidProxy* Proxy;

	// The delegate used to register substepped physics
	FCalculateCustomPhysics CalculateCustomPhysics;

	static int32 ms_NextInstanceId;

    static const int MPM_THREADS = 256;
    static const int RADIX_THREADS = 128;
    static const int HISTO_WIDTH = 32;

	static const int STATISTICS_PER_MANIPULATOR = 8;
	//static const int DENSITY_COMPUTE_BLOCK = 5;
	
	const int BlockDim = 8;

	FIntPoint CurrentTextureResolution = FIntPoint(2048,2048);
	int32 OldWidth = 0;
	int32 OldHeight = 0;

	int32 OldWidthGameThread = 0;
	int32 OldHeightGameThread = 0;

	bool bNeedRegister2dTextures = true;
	bool b2DTexturesDirty = true;
	bool bIsDirty = true;
	bool bIsDirtyGameThread = true;
	bool b3DTexturesDirty = true;
    bool bInitialized = false;
    bool bIsEnabled = false;
	bool bHaveEmitter = false;
	bool bIsDeleting = false;

    int NumParticles = 0;

    float ParticleMass = 1.0f;

	float ParticleDiameter = 10.0f;
    float Timestep = 0.0f;
    FIntVector GridSize = { 0, 0, 0 };

	FVector ContainerPosition = { 0.0f, 0.0f, 0.0f };
	FVector FluidInitialVelocity;
    float SimulationInternalTime = 0.0f;
    int SimulationInternalFrame = 0;
    int NumNodes = 0;
    int NumParticlesRounded = 0;
    int RadixWorkGroups = 0; //Radix sort workgroups count

    FSimulationParams FluidParameters;

    FCameraParams CameraRenderParams;
	FMeshRenderGlobalParams MeshRenderGlobalParams;
    FRenderParams RenderParams;

	FRegisterSolverBuffersBridgeParams RegisterSolverBuffersBridgeParams;
	FRegisterManipulatorsBridgeParams RegisterManipulatorsBridgeParams;
	FRegisterRenderResourcesBridgeParams RegisterRenderResourcesBridgeParams;
	FRegisterParticlesBuffersBridgeParams RegisterParticlesBuffersParams;
	FInitializeGPUReadbackParams InitializeGPUReadbackParams;

	UPROPERTY(EditAnywhere,  meta = (Category = "Zibra Liquid Solver Parameters"))
    FZibraLiquidSolverParams SolverParameters;

	UPROPERTY(EditAnywhere,  meta = (Category = "Zibra Liquid Material Parameters"))
	FZibraLiquidMaterialParams MaterialParameters;

	UPROPERTY(EditAnywhere, meta = (Category = "Zibra Liquid Advanced Render Parameters"))
	FZibraLiquidAdvancedRenderParameters RenderingParameters;

	struct FInitZibraTexture2DDynamic
	{
		UZibraTexture2DDynamic* Color0UT;
		UZibraTexture2DDynamic* DepthUT;
		FTextureBridge* Color0Bridge;
		FTextureBridge* DepthBridge;
		int32 Width;
		int32 Height;
	};

	struct FInitRHIResourcesData
	{
		UTextureRenderTargetVolume* RenderTargetGridNormals;
		UTextureRenderTargetVolume* RenderTargetGridDensity;
		FTextureBridge* GridNormalTextureBridge;
		FTextureBridge* DensityTextureBridge;
	};

	static void SetZibra2DDynamic(UZibraTexture2DDynamic* Reference, UZibraTexture2DDynamic* Texture);
	static void InitRHIResources(FInitRHIResourcesData data, FRegisterSolverBuffersBridgeParams* RegisterSolverBuffersBridgeParams);

	TArray<UZibraSDFCollider*> AllZibraSDFColliders;
	TArray<UZibraManipulator*> AllManipulators;
	
	FDelegateHandle OnPostResolvedSceneColorHandle;

	UPROPERTY()
	UZibraManipulatorManager* ManipulatorManager;

    void SetFluidParameters();
    void CreateBuffers();
	void ReleaseGPUResources();
    void InitializeParameters();
	void InitializeManipulators();
	FRHIResourceCreateInfo GetCreateInfo(TCHAR* DebugName, FResourceArrayInterface* Data);
	FRHIResourceCreateInfo GetCreateInfo(TCHAR* DebugName);

	float DivergenceDecayCurve(float x);
	void UpdateManipulatorStatistics();
	void CleanupBeforeBeginPlay();

	UPROPERTY()
	bool bSpawnedEmitterActor = false;
	
	void SpawnEmitterActor();
	void UpdateCamera(FPostOpaqueRenderParameters& Parameters);
	void UpdateNativeTextures();
	void UpdateNativeRenderParams(FPostOpaqueRenderParameters& Parameters);
	static void StepPhysics(ZibraLiquidProxy* Proxy, int IterationsPerFrame, FSimulationParams FluidParameters, const TArray<FManipulatorParams>& ManipulatorParams, int ManipulatorManagerElements);
	void UpdateMaterial();
	ZibraMatrix CalculateEyeRayCameraCoeficients(ZibraMatrix ViewMatrix);
	int32 GetParticleCountRounded();
	
	FIntPoint ApplyDownscaleFactor(FIntPoint val);

	static void UpdateCameraRenderParamsRT(ZibraLiquidProxy* Proxy, FCameraParams CameraRenderParams, FMeshRenderGlobalParams MeshRenderGlobalParams);
	static void RegisterRenderResourcesRT(ZibraLiquidProxy* Proxy,  FRegisterRenderResourcesBridgeParams RegisterRenderResourcesBridgeParams);
	static void SetNativeRenderParamsRT(ZibraLiquidProxy* Proxy, FRenderParams RenderParams);

	static void DisableUAVOverlap(FRHICommandListImmediate& RHICmdList);
	bool CheckDll();
	void ValidateGravity();


#if WITH_EDITORONLY_DATA
protected:
	/** Billboard used to see the scene in the editor */
	UPROPERTY()
		UBillboardComponent* SpriteComponent;
#endif
};