// Copyright 2020 Wouter Weynants. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptInterface.h"
#include "Engine/BlendableInterface.h"
#include "Engine.h"
#include "GameFramework/Info.h"
#include "Misc/DateTime.h"
#include "HemisphereTimelapse.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "UObject/SoftObjectPath.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Components/PostProcessComponent.h"
#include "HemisphereSky.generated.h"

UENUM(BlueprintType)
enum class ELoopType : uint8
{
	LP_Loop 	UMETA(DisplayName = "Loop"),
	LP_NoLoop 	UMETA(DisplayName = "Don't Loop"),
	LP_LoopPingPong	UMETA(DisplayName = "Loop with Ping-Pong")
};

UENUM(BlueprintType)
enum class EPlaybackType : uint8
{
	PT_Forward 	UMETA(DisplayName = "Forward"),
	PT_Backward 	UMETA(DisplayName = "Backward")
};

/** Enable or disable the updating of SkyLightComponent values */
USTRUCT(BlueprintType)
struct HEMISPHERESKIES_API FSkyLightUpdateSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyLight Update")
	bool SetCubemapBlend;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyLight Update")
	bool SetIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyLight Update")
	bool SetLightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyLight Update")
	bool SetSourceCubemapAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkyLight Update")
	bool RecaptureSkyAfterRotating;

	FSkyLightUpdateSettings()
	{
		SetCubemapBlend = true;
		SetIntensity = true;
		SetLightColor = true;
		SetSourceCubemapAngle = true;
		RecaptureSkyAfterRotating = true;
	}
};

/** Enable or disable the updating of ExponentialHeightFogComponent values */
USTRUCT(BlueprintType)
struct HEMISPHERESKIES_API FFogUpdateSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog Update")
	bool SetDirectionalInscatteringColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog Update")
	bool SetFogInscatteringColor;

	FFogUpdateSettings()
	{
		SetDirectionalInscatteringColor = true;
		SetFogInscatteringColor = true;
	}
};

/** Enable or disable the updating of DirectionalLightComponent values */
USTRUCT(BlueprintType)
struct HEMISPHERESKIES_API FSunUpdateSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Update")
	bool SetRelativeRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Update")
	bool SetIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Update")
	bool SetCastShadows;

	FSunUpdateSettings()
	{
		SetRelativeRotation = true;
		SetIntensity = true;
		SetCastShadows = true;
	}
};

UCLASS(Blueprintable, AutoCollapseCategories = (Info, Rendering, Replication, Collision, Input, Actor), AutoExpandCategories = (Sky, Sun, Timelapse, Fog))
class HEMISPHERESKIES_API AHemisphereSky : public AActor
{
	GENERATED_BODY()

public:	

	FStreamableManager StreamableManager;

	/** Hemisphere controlled SkyLight */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkyLightComponent* SkyLight = nullptr;

	/** Hemisphere controlled SunLight */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UDirectionalLightComponent* SunLight = nullptr;

	/** Hemisphere controlled ExponentialHeightFog */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UExponentialHeightFogComponent* ExponentialHeightFog = nullptr;

	/** Hemisphere controlled StaticMesh skybox */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* Sky = nullptr;

	/** Hemisphere controlled SkyAtmosphere */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USkyAtmosphereComponent* SkyAtmosphere = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class UPostProcessComponent* PostProcessVolume = nullptr;

	/** Start date and time of the selected sequence */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Info", meta = (editcondition = "false"))
	FDateTime StartTime;

	/** End date and time of the selected sequence */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Info", meta = (editcondition = "false"))
	FDateTime EndTime;

	/** Current date and time of the selected sequence */
	UPROPERTY(EditAnywhere, Category = "Info", meta = (editcondition = "false"))
	FDateTime CurrentDateTime;

	UPROPERTY(Transient)
	FHemisphereMoment CurrentMoment;

	/** Enable or disable the updating of SkyLightComponent values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Update")
	FSkyLightUpdateSettings SkyLightUpdateSettings;

	/** Enable or disable the updating of ExponentialHeightFog values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Update")
	FFogUpdateSettings FogUpdateSettings;

	/** Enable or disable the updating of DirectionalLightComponent values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Update")
	FSunUpdateSettings SunUpdateSettings;

	/** The selected timelapse that drives the Hemisphere system */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timelapse")
	UHemisphereTimelapse* Timelapse;

	/**
	* Current time.
	* Can be modified with the Sequencer, Hemisphere's own AutoPlay or through BP.
	* 0.0 = Start of sequence.
	* 1.0 = End of sequence.
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Timelapse", meta = (AllowPrivateAccess = "true", ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
	float Time;

	/** Playback sequence when true */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Timelapse")
	bool AutoPlay = true;

	/** Playback direction */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Timelapse")
	EPlaybackType PlaybackType = EPlaybackType::PT_Forward;

	/**
	* Speed at which the Timelapse is played.
	* 1.0 = Real life speed
	* 10.0 = 10 times faster than real life speed
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Timelapse", meta = (AllowPrivateAccess = "true", editcondition = AutoPlay))
	float PlaySpeed = 10.f;

	/** Determines what to do when the end of a sequence is encountered. If not looping, the sequence will stop at the end. */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Timelapse", meta = (AllowPrivateAccess = "true", editcondition = AutoPlay))
	ELoopType Loop = ELoopType::LP_LoopPingPong;

	/** Whether to play in the editor's viewport */
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Timelapse", meta = (AllowPrivateAccess = "true", editcondition = AutoPlay))
	bool PlayInEditor = true;

	/** Multiply the sun's intensity */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sun")
	float SunLightIntensity = 1.0f;

	/**
	* 0.0 = Sun shines at recorded sun intensity. This means the SkyLight + Sunlight will be outputting more light than was present in real life.
	* 1.0 = Sun shines at recorded sun instensity MINUS recorded sky intensity. This is most accurately but when cloudy the directional light is not shining at all.
	*/
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Sun")
	float SunIlluminanceSubtraction = 0.75f;

	/** Clamps the DirectionalLightComponent's maximum value */
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Sun")
	float MaxSunLightIntensity = 200000.0f;

	/** Scale of the sky box StaticMesh */
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Sky")
	float SkyRadius = 100000.0f;

	/** Multiply the intensity of the SkyLight */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float SkyLightIntensity = 1.0f;

	/** 
	* Multiply the intensity of the Sky Material.
	* 1.0 is the colour-science correct value.
	* Higher values can provide a nice filmic look.
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float SkyBoxIntensity = 1.0f;

	/** Multiply the Sky Material and SkyLight colors */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	FLinearColor SkyColorMultiply = FLinearColor::White;

	/** Change the skybox saturation */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float SkyColorSaturation = 1.0f;

	/** Add contrast to the skybox colors */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float SkyColorContrast = 1.0f;

	/** Sets the Lower Hemisphere Color of the SkyLightComponent to (AverageColor * LowerHemisphereColorMultiply) */
	UPROPERTY(AdvancedDisplay, Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	bool SetLowerHemisphereColor = true;

	/** Multiplies the average color to construct the Lower Hemisphere Color of the SkyLightComponent */
	UPROPERTY(AdvancedDisplay, Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky", meta = (editcondition = SetLowerHemisphereColor))
	FLinearColor LowerHemisphereColorMultiply = FLinearColor(0.125f,0.125f,0.125f,1.0f);

	/** 
	* Current sequences are not yet recorded in a real-life location with no obstructions on the horizon.
	* Because of this by default a part of the image gets clipped out to prevent trees and electrical wiring from showing.
	* Higher values push down the horizon.
	* 0.0 = Most accurate and realistic
	* 5.0 = Very useful in most cases, as it masks out nearly all obstructions to present a clear view of the sky
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Sky")
	float ElevationCorrection = 0.0f;


	/** Fog inscattering */
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Fog")
	float Inscatter = 1.0f;

	/** 
	* Directional inscattering.
	* If the fog seems to be flickering according to the sun's intensity it's most likely due to this value.
	*/
	UPROPERTY(Interp, EditAnywhere, BlueprintReadWrite, Category = "Fog")
	float DirectionalInscatter = 0.0f;

	/** Saturation of the average color */
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Color")
	float AverageColorSaturation = 1.0f;

	/** Average color hue shift */
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Color")
	float AverageColorHueShift = 0.f;

	/** Average color value */
	UPROPERTY(Interp, EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Color")
	float AverageColorValue = 1.0f;

	/** Color Science Constant K */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Info", meta = (editcondition = "false"))
	float ConstantK = 12.5f;

	/** Sky Luminance expressed in cd/m2 */
	UPROPERTY(EditAnywhere, Category = "Info", meta = (editcondition = "false"))
	float SkyLuminance = 0.0f;

	/** Sky Illuminance expressed in Lux. Reconstructed from camera light sensor. */
	UPROPERTY(EditAnywhere, Category = "Info", meta = (editcondition = "false"))
	float SkyIlluminance = 0.0f;

	/** Sun Illuminance expressed in Lux. Captured with solar intensity data logger. */
	UPROPERTY(EditAnywhere, Category = "Info", meta = (editcondition = "false"))
	float SunIlluminance = 0.0f;

	/**
	* Enable Hemisphere-controlled exposure settings through the attached PostProcessComponent.
	* Only works when extended luminance range is enabled
	*/
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = ExtendedLuminanceRange))
	bool HemisphereAutoExposure = true;

	/** The interpolated exposure of the recorded HDR photograph, using ExposureInterpSpeed */
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = "false"))
	float Exposure = 0.0f;

	/** The exposure of the recorded HDR photograph */
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = "false"))
	float ExposureTarget = 0.0f;

	/** Speed at which Exposure interpolates to ExposureTarget, change this variable according to your playback speed and if you encounter any flickering */
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = HemisphereAutoExposure))
	float ExposureInterpSpeed = 10.0f;

	/**
	* AutoExposure = Exposure - ExposureOffset
	* Post Process Volume's AutoExposureMinBrightness =  AutoExposure
	* Post Process Volume's AutoExposureMaxBrightness = AutoExposure + ExposureRange
	*/
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = HemisphereAutoExposure))
	float ExposureRange = 0.0f;

	/**
	* AutoExposure = Exposure - ExposureOffset
	* Post Process Volume's AutoExposureMinBrightness =  AutoExposure
	* Post Process Volume's AutoExposureMaxBrightness = AutoExposure + ExposureRange
	*/
	UPROPERTY(EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = HemisphereAutoExposure))
	float ExposureOffset = 0.5f;

	/** Clamps AutoExposure values */
	UPROPERTY(AdvancedDisplay, EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = HemisphereAutoExposure))
	float ExposureClampMin = 5.0f;

	/** Clamps AutoExposure values */
	UPROPERTY(AdvancedDisplay, EditAnywhere, Category = "Post Process | Exposure", meta = (editcondition = HemisphereAutoExposure))
	float ExposureClampMax = 16.0f;

	/**
	* By default UE4 does not make use of its extended luminance range.
	* Change this setting in Project Settings -> Rendering -> Defaults then restart the editor.
	* This setting will automatically adjust accordingly
	*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Lighting")
	bool ExtendedLuminanceRange = false;

	/** If the luminance range is not extended, divide the total light emitted by the sky and sun by this value */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Lighting")
	float PhysicallyInaccurateLuminanceRangeFraction = 1000.0;

	/** Multiplier of all light-emitting components and sky material intensity */
	UPROPERTY(EditAnywhere, Category = "Lighting")
	float GlobalIntensity = 1.0f;

	UPROPERTY(Transient)
	bool TimelapseInitialized = false;

	UPROPERTY(Transient)
	TArray<UTexture*> TexturesInMemory;

	float GetSunIlluminance();
	float GetSkyIlluminance();
	float GetSkyLuminance();

	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	FRotator GetSunRotation();

	/** Gets the current sun elevation */
	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	float GetSunElevation();
	FDateTime GetCurrentDateTime();

	/** Gets the current moment */
	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	FHemisphereMoment GetCurrentMoment();

	/** Gets the next moment */
	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	FHemisphereMoment GetNextMoment();

	float GetSkyLightRotation();
	FLinearColor GetAverageColor();
	float GetInBetween();


	AHemisphereSky();

	UFUNCTION()
	void UnloadIrrelevantTextures();

	void UnloadMomentTextures(FHemisphereMoment Moment);

	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

	void UpdateInfo(float DeltaTime);

protected:
	virtual void BeginPlay() override;
	static FString AddLeadingZeroes(const uint32& Number, const int Length);
	void OnTimelapseChanged(const FName& PropertyName);
	void OnSkyLowerHemisphereColorChanged(const FName& PropertyName);
	void OnHemisphereAutoExposureChanged(const FName& PropertyName);
	void MoveTime(float DeltaTime);
	void AddComponents();
	void UpdateSun();
	void UpdateSunIntensity();
	void UpdateSunRotation();
	void OnSkyRadiusChanged(const FName& PropertyName);
	uint32 GetMomentNumberFromTime(float GivenTime);
	void UpdateSkyLight();
	void PreloadTexture(FSoftObjectPath SoftObjectPath);
	float GetBaseIntensity();
	
#if WITH_EDITOR
	virtual void PostInterpChange(FProperty* PropertyThatChanged) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UMaterialInstanceDynamic* GetSkyMaterial();
	UHemisphereTimelapse* GetDefaultTimelapse();
	void UpdateSkyMaterial();
	void UpdateTextures();
	bool HasMomentChanged();
	UTexture2D* DynamicGetTexture(TSoftObjectPtr<UTexture2D> Texture);
	UTextureCube* DynamicGetTextureCube(TSoftObjectPtr<UTextureCube> Texture);
	void UpdateFog();
	void AddSunDirectionalLightComponent();
	void AddSkyStaticMeshComponent();
	void UpdateSkyMeshRadius();
	void AddSkyLightComponent();
	void InitializeTimelapse();
	void AttachComponentToRoot(USceneComponent* ComponentToAttach);
	void AddFogComponent();
	void AddSkyAtmosphereComponent();
	void AddPostProcessComponent();
	void UpdatePostProcessVolume();
	FHemisphereMoment GetMoment(int32 Offset);
	float GetLuminance(float exposure);
	float GetIlluminance(float exposure);
	void SetAutoExposureEnabled();
};
