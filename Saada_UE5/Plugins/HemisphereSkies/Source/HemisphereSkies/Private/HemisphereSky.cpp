// Copyright 2020 Wouter Weynants. All Rights Reserved.

#include "HemisphereSky.h"
#include "Materials/MaterialInstance.h"
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "HemisphereSettings.h"
#include "Engine/AssetManager.h"

float AHemisphereSky::GetSkyLuminance()
{
	return GetLuminance(Exposure + Timelapse->NDFilterStops) * GetBaseIntensity();
}

float AHemisphereSky::GetSkyIlluminance()
{
	return GetIlluminance(Exposure + Timelapse->NDFilterStops);
}

float AHemisphereSky::GetSunIlluminance()
{
	// https://ieee-dataport.org/open-access/conversion-guide-solar-irradiance-and-lux-illuminance
	float base_illuminance = FMath::Lerp(GetCurrentMoment().SolarIlluminance, GetNextMoment().SolarIlluminance, GetInBetween()) * 120.0f;
	return base_illuminance * SunLightIntensity * GetBaseIntensity();
}

AHemisphereSky::AHemisphereSky()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	AddComponents();
	if (!Timelapse)
	{
		Timelapse = GetDefaultTimelapse();
	}

	static const auto VarDefaultAutoExposureExtendDefaultLuminanceRange = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DefaultFeature.AutoExposure.ExtendDefaultLuminanceRange"));
	ExtendedLuminanceRange = VarDefaultAutoExposureExtendDefaultLuminanceRange->GetValueOnGameThread() == 1;

	ExposureOffset = 2.0f;

	SetAutoExposureEnabled();
}

void AHemisphereSky::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle UnloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(UnloadTimerHandle, this, &AHemisphereSky::UnloadIrrelevantTextures, GetDefault<UHemisphereSettings>()->DynamicTextureUnLoadFrequency, false);
}

void AHemisphereSky::Destroyed()
{
	Super::Destroyed();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AHemisphereSky::InitializeTimelapse()
{
	if (Timelapse && Timelapse->Moments.Num() > 0)
	{
#if WITH_EDITOR
		if (!Timelapse->IsPluginVersionCompatible())
		{
			FText DialogText = FText::FromString("The selected sequence is incompatible with the installed version of Hemisphere, please update your sequence assets to the latest version.");
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		}
#endif
		StartTime = Timelapse->Moments[0].Time;
		EndTime = Timelapse->Moments[Timelapse->Moments.Num() - 1].Time;

		GetCurrentDateTime();

		Exposure = GetCurrentMoment().EV;

		TimelapseInitialized = true;

		TexturesInMemory.Empty();

		if (!GetDefault<UHemisphereSettings>()->DynamicTextureLoading)
		{
#if WITH_EDITOR
			FString TimelapseLoadTaskText = "Loading " + Timelapse->GetName() + " Textures";
			FScopedSlowTask TimelapseLoadTask(Timelapse->Moments.Num(), FText::FromString(TimelapseLoadTaskText));
			TimelapseLoadTask.MakeDialog(false);
#endif
			for (auto Moment : Timelapse->Moments)
			{
#if WITH_EDITOR
				TimelapseLoadTask.EnterProgressFrame(1, FText::FromString(TimelapseLoadTaskText));
				TimelapseLoadTaskText = "Loading " + Timelapse->GetName() + " Textures - " + Moment.Sky.GetAssetName();
#endif
				TexturesInMemory.Add(DynamicGetTexture(Moment.Sky));
				TexturesInMemory.Add(DynamicGetTexture(Moment.Motion));
				TexturesInMemory.Add(DynamicGetTextureCube(Moment.Cubemap));
			}
		}

		SunLight->bAffectsWorld = true;
		SkyLight->bAffectsWorld = true;

	}
}

void AHemisphereSky::AddComponents()
{
	AddSkyStaticMeshComponent();
	AddSkyLightComponent();
	AddSunDirectionalLightComponent();
	AddFogComponent();
	AddSkyAtmosphereComponent();
	AddPostProcessComponent();
}

void AHemisphereSky::AddPostProcessComponent()
{
	PostProcessVolume = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessVolume"));
	PostProcessVolume->bEnabled = true;
	PostProcessVolume->Settings.AutoExposureBias = 0;
	PostProcessVolume->Settings.AutoExposureMethod = EAutoExposureMethod::AEM_Histogram;
	PostProcessVolume->Settings.bOverride_AutoExposureMinBrightness = true;
	PostProcessVolume->Settings.bOverride_AutoExposureMaxBrightness = true;
	AttachComponentToRoot(PostProcessVolume);
}

void AHemisphereSky::AddFogComponent()
{
	ExponentialHeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	ExponentialHeightFog->FogHeightFalloff = 0.1f;
	ExponentialHeightFog->FogDensity = 0.005f;
	ExponentialHeightFog->FogMaxOpacity = 1.0f;
	ExponentialHeightFog->VolumetricFogScatteringDistribution = 0.5f;
	ExponentialHeightFog->VolumetricFogExtinctionScale = 3.0f;
	AttachComponentToRoot(ExponentialHeightFog);
}

void AHemisphereSky::AddSkyAtmosphereComponent()
{
	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->AerialPespectiveViewDistanceScale = 0.0f;
	SkyAtmosphere->HeightFogContribution = 0.0f;
	AttachComponentToRoot(SkyAtmosphere);
}

void AHemisphereSky::AddSkyStaticMeshComponent()
{
	Sky = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sky"));
	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/HemisphereSkies/Meshes/SM_Sky.SM_Sky'"));
	if (MeshAsset.Object != nullptr)
	{
		Sky->SetStaticMesh(MeshAsset.Object);
		Sky->CastShadow = false;
		Sky->SetCollisionProfileName("NoCollision");
		Sky->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sky->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	}
	UpdateSkyMeshRadius();
	RootComponent = Sky;
}

void AHemisphereSky::AddSunDirectionalLightComponent()
{
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
#if ENGINE_MAJOR_VERSION < 5
	SunLight->bUsedAsAtmosphereSunLight = true;
#endif
	SunLight->bEnableLightShaftOcclusion = true;
	SunLight->OcclusionMaskDarkness = 0.5f;
	SunLight->bEnableLightShaftBloom = false;
	SunLight->DynamicShadowDistanceMovableLight = 20000.0f;
	SunLight->bUseRayTracedDistanceFieldShadows = true;
	SunLight->DistanceFieldShadowDistance = 250000.0f;
	SunLight->bUseTemperature = true;
	SunLight->bAffectDynamicIndirectLighting = true;
	SunLight->DisabledBrightness = 1.0f;
	SunLight->ShadowDistanceFadeoutFraction = 1.0f;
	SunLight->bAffectsWorld = false;
}

void AHemisphereSky::AddSkyLightComponent()
{
	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SourceType = ESkyLightSourceType::SLS_SpecifiedCubemap;
	SkyLight->Mobility = EComponentMobility::Movable;
	SkyLight->OcclusionMaxDistance = 3000.0f;
	SkyLight->MinOcclusion = 0.25f;
	SkyLight->bCaptureEmissiveOnly = true;
	SkyLight->bLowerHemisphereIsBlack = true;
	SkyLight->bAffectTranslucentLighting = true;
	SkyLight->bTransmission = true;
	SkyLight->bAffectsWorld = false;
	AttachComponentToRoot(SkyLight);
}

void AHemisphereSky::AttachComponentToRoot(USceneComponent* ComponentToAttach)
{
	ComponentToAttach->SetupAttachment(Sky);
}

void AHemisphereSky::UpdateSun()
{
	UpdateSunRotation();
	UpdateSunIntensity();
}

void AHemisphereSky::UpdatePostProcessVolume()
{
	float AutoExposure = Exposure - ExposureOffset;
	PostProcessVolume->Settings.AutoExposureMinBrightness = FMath::Clamp(AutoExposure, ExposureClampMin, ExposureClampMax);
	PostProcessVolume->Settings.AutoExposureMaxBrightness = FMath::Clamp(AutoExposure + ExposureRange, ExposureClampMin, ExposureClampMax);
}

void AHemisphereSky::UpdateSunIntensity()
{
	float Sunset = (Timelapse->GetSunInfo(GetCurrentDateTime()).CorrectedElevation - ElevationCorrection) - 180;
	Sunset = FMath::Clamp(Sunset, 0.0f, 180.0f) / 180.0f;

	if (Sunset <= 0.0f)
	{
		if (SunUpdateSettings.SetCastShadows && SunLight->CastShadows != false) { SunLight->CastShadows = false; }
		if (SunUpdateSettings.SetIntensity && SunLight->Intensity != 0.0f) { SunLight->SetIntensity(0.0f); }
	}
	else
	{
		if (SunUpdateSettings.SetCastShadows)
		{
			if (SunLight->CastShadows == false)
			{
				SunLight->CastShadows = true;
			}
		}

		if (SunUpdateSettings.SetIntensity)
		{
			float i = GetSkyIlluminance() / FMath::Pow(2, Timelapse->NDFilterStops);
			float s = GetSunIlluminance();
			SunLight->SetIntensity(FMath::Clamp(FMath::Lerp(s, s - i, SunIlluminanceSubtraction), 0.0f, MaxSunLightIntensity));
		}
	}
}

void AHemisphereSky::UpdateSunRotation()
{
	if (SunUpdateSettings.SetRelativeRotation)
	{
		SunLight->SetRelativeRotation(GetSunRotation());
	}
}

void AHemisphereSky::UnloadIrrelevantTextures()
{
	if (TimelapseInitialized)
	{
		TArray<FDateTime> RelevantMomentDateTimes;

		if (Timelapse->Moments.Num() > 0)
		{
			RelevantMomentDateTimes.Add(GetMoment(-2).Time);
			RelevantMomentDateTimes.Add(GetMoment(-1).Time);
			RelevantMomentDateTimes.Add(GetCurrentMoment().Time);
			RelevantMomentDateTimes.Add(GetNextMoment().Time);
			RelevantMomentDateTimes.Add(GetMoment(2).Time);
		}

		if (RelevantMomentDateTimes.Num() > 0)
		{
			for (FHemisphereMoment Moment : Timelapse->Moments)
			{
				if (!RelevantMomentDateTimes.Contains(Moment.Time))
				{
					UnloadMomentTextures(Moment);
				}
			}
		}
	}
}

void AHemisphereSky::UnloadMomentTextures(FHemisphereMoment Moment)
{
	if (Moment.Sky.IsValid())
	{
		StreamableManager.Unload(Moment.Sky.ToSoftObjectPath());
	}
	if (Moment.Motion.IsValid())
	{
		StreamableManager.Unload(Moment.Motion.ToSoftObjectPath());
	}
	if (Moment.Cubemap.IsValid())
	{
		StreamableManager.Unload(Moment.Cubemap.ToSoftObjectPath());
	}
}

FRotator AHemisphereSky::GetSunRotation()
{
	return FRotator(GetSunElevation(), Timelapse->GetSunInfo(GetCurrentDateTime()).Azimuth + GetActorRotation().Yaw, 0.0f);
}

float AHemisphereSky::GetSunElevation()
{
	return Timelapse->GetSunInfo(GetCurrentDateTime()).CorrectedElevation - ElevationCorrection;
}

FDateTime AHemisphereSky::GetCurrentDateTime()
{
	CurrentDateTime = GetCurrentMoment().Time + (FTimespan(GetNextMoment().Time - GetCurrentMoment().Time) * GetInBetween());
	return CurrentDateTime;
}

void AHemisphereSky::UpdateSkyMeshRadius()
{
	Sky->SetRelativeScale3D(FVector::OneVector * SkyRadius);
}

void AHemisphereSky::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TimelapseInitialized)
	{
		MoveTime(DeltaTime);

		UpdateInfo(DeltaTime);

		UpdateSkyLight();
		UpdateSkyMaterial();
		UpdateFog();
		UpdateSun();
		UpdatePostProcessVolume();

		UpdateTextures();
	}
	else
	{
		InitializeTimelapse();
	}
}

void AHemisphereSky::UpdateInfo(float DeltaTime)
{
	ExposureTarget = FMath::Lerp(GetCurrentMoment().EV, GetNextMoment().EV, GetInBetween()) - Timelapse->NDFilterStops;
	Exposure = FMath::FInterpTo(Exposure, ExposureTarget, DeltaTime, ExposureInterpSpeed);

	SkyLuminance = GetSkyLuminance() / FMath::Pow(2, Timelapse->NDFilterStops);
	SkyIlluminance = GetSkyIlluminance() / (FMath::Pow(2, Timelapse->NDFilterStops));
	SunIlluminance = GetSunIlluminance();
}

void AHemisphereSky::MoveTime(float DeltaTime)
{
	if (AutoPlay)
	{
		float TotalTime = (EndTime - StartTime).GetTotalSeconds();

		if (Time > 1.0f)
		{
			if (Loop == ELoopType::LP_NoLoop)
			{
				Time = 1.0f;
				AutoPlay = false;
			}
			else if (Loop == ELoopType::LP_Loop)
			{
				PlaybackType = EPlaybackType::PT_Forward;
				Time = 0.0f;
			}
			else if (Loop == ELoopType::LP_LoopPingPong)
			{
				PlaybackType = EPlaybackType::PT_Backward;
			}
		}
		else if (Time < 0.0f)
		{
			if (Loop == ELoopType::LP_NoLoop)
			{
				Time = 0.0f;
				AutoPlay = false;
			}
			else if (Loop == ELoopType::LP_Loop)
			{
				PlaybackType = EPlaybackType::PT_Backward;
				Time = 1.0f;
			}
			else if (Loop == ELoopType::LP_LoopPingPong)
			{
				PlaybackType = EPlaybackType::PT_Forward;
			}
		}

		if (PlaybackType == EPlaybackType::PT_Forward)
		{
			Time += DeltaTime / TotalTime * PlaySpeed;
		}


		if (PlaybackType == EPlaybackType::PT_Backward)
		{
			Time -= DeltaTime / TotalTime * PlaySpeed;
		}
	}
}

FHemisphereMoment AHemisphereSky::GetCurrentMoment()
{
	FHemisphereMoment Moment = GetMoment(0);
	return Moment;
}

FHemisphereMoment AHemisphereSky::GetNextMoment()
{
	FHemisphereMoment Moment = GetMoment(1);
	return Moment;
}

FHemisphereMoment AHemisphereSky::GetMoment(int32 Offset)
{
	int32 Selection = FMath::Clamp(int32(GetMomentNumberFromTime(Time) + Offset), 0, Timelapse->Moments.Num() - 1);
	return Timelapse->Moments[Selection];
}

UMaterialInstanceDynamic* AHemisphereSky::GetSkyMaterial()
{
	return Cast<UMaterialInstanceDynamic>(Sky->GetMaterial(0));
}

#if WITH_EDITOR
void AHemisphereSky::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName(PropertyChangedEvent.Property->GetFName());
		OnSkyRadiusChanged(PropertyName);
		OnTimelapseChanged(PropertyName);
		OnSkyLowerHemisphereColorChanged(PropertyName);
		OnHemisphereAutoExposureChanged(PropertyName);
	}
}
#endif

void AHemisphereSky::OnTimelapseChanged(const FName& PropertyName)
{
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHemisphereSky, Timelapse))
	{
		TimelapseInitialized = false;
		InitializeTimelapse();
	}
}

void AHemisphereSky::OnSkyLowerHemisphereColorChanged(const FName& PropertyName)
{
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHemisphereSky, SetLowerHemisphereColor))
	{
		SkyLight->bLowerHemisphereIsBlack = SetLowerHemisphereColor;
		SkyLight->RecaptureSky();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHemisphereSky, LowerHemisphereColorMultiply))
	{
		SkyLight->RecaptureSky();
	}
}

void AHemisphereSky::OnHemisphereAutoExposureChanged(const FName& PropertyName)
{
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHemisphereSky, HemisphereAutoExposure))
	{
		SetAutoExposureEnabled();
	}
}

void AHemisphereSky::SetAutoExposureEnabled()
{
	if (ExtendedLuminanceRange)
	{
		PostProcessVolume->bEnabled = HemisphereAutoExposure;
	}
	else
	{
		PostProcessVolume->bEnabled = false;
	}
}

#if WITH_EDITOR
void AHemisphereSky::PostInterpChange(FProperty * PropertyThatChanged)
{
	Super::PostInterpChange(PropertyThatChanged);

	if (PropertyThatChanged != nullptr)
	{
		const FName PropertyName(PropertyThatChanged->GetFName());
		OnSkyRadiusChanged(PropertyName);
		OnTimelapseChanged(PropertyName);
	}
}
#endif

#if WITH_EDITOR
bool AHemisphereSky::ShouldTickIfViewportsOnly() const
{
	return PlayInEditor;
}
#endif

void AHemisphereSky::OnSkyRadiusChanged(const FName& PropertyName)
{
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHemisphereSky, SkyRadius) && TimelapseInitialized)
	{
		UpdateSkyMeshRadius();
	}
}

uint32 AHemisphereSky::GetMomentNumberFromTime(float GivenTime)
{
	uint32 Selection = Timelapse->Moments.Num() - 2;
	auto CurrentMomentNumber = (Selection * GivenTime);
	return FMath::TruncToInt(CurrentMomentNumber);
}

void AHemisphereSky::UpdateSkyLight()
{
	if (SkyLight->SourceType == ESkyLightSourceType::SLS_SpecifiedCubemap)
	{
		PreloadTexture(GetMoment(-1).Sky.ToSoftObjectPath());
		PreloadTexture(GetMoment(2).Sky.ToSoftObjectPath());

		if (SkyLightUpdateSettings.SetCubemapBlend)
		{
			SkyLight->SetCubemapBlend(DynamicGetTextureCube(GetCurrentMoment().Cubemap), DynamicGetTextureCube(GetNextMoment().Cubemap), GetInBetween());
		}

		if (SkyLightUpdateSettings.SetIntensity)
		{
			SkyLight->SetIntensity(GetSkyLuminance() * FMath::Pow(2, Timelapse->NDFilterStops) * SkyLightIntensity);
		}

		if (SkyLight->SourceCubemapAngle != GetSkyLightRotation())
		{
			if (SkyLightUpdateSettings.SetSourceCubemapAngle)
			{
				SkyLight->SourceCubemapAngle = GetSkyLightRotation();
				if (SkyLightUpdateSettings.RecaptureSkyAfterRotating) { SkyLight->RecaptureSky(); }
			}
		}

		if (SetLowerHemisphereColor)
		{
			SkyLight->SetLowerHemisphereColor(LowerHemisphereColorMultiply * GetAverageColor());
		}
	}

	
	if (SkyLightUpdateSettings.SetLightColor) { SkyLight->SetLightColor(SkyColorMultiply); }
}

void AHemisphereSky::PreloadTexture(FSoftObjectPath SoftObjectPath)
{
	if (!SoftObjectPath.IsValid() && GetDefault<UHemisphereSettings>()->LoadAsync && GetDefault<UHemisphereSettings>()->DynamicTextureLoading)
	{
		StreamableManager.RequestAsyncLoad(SoftObjectPath);
	}
}

float AHemisphereSky::GetBaseIntensity()
{
	float extended_luminance_multiplier = (1.0f / PhysicallyInaccurateLuminanceRangeFraction);
	if (ExtendedLuminanceRange)
	{
		extended_luminance_multiplier = 1.0f;
	}
	return GlobalIntensity * extended_luminance_multiplier;
}

float AHemisphereSky::GetSkyLightRotation()
{
	float angle = (360.0f + 270.0f - this->GetActorRotation().Yaw);
	angle = FMath::Fmod(angle, 360.0f);
	return angle;
}

void AHemisphereSky::UpdateSkyMaterial()
{
	if (GetSkyMaterial() == nullptr)
	{
		UMaterialInterface* Material;
		UMaterialInstanceDynamic* Instance;

		Material = Sky->GetMaterial(0);
		Instance = Sky->CreateDynamicMaterialInstance(0, Material);
	}

	GetSkyMaterial()->SetScalarParameterValue("InBetween", GetInBetween());
	GetSkyMaterial()->SetVectorParameterValue("AverageColor", GetAverageColor());
	GetSkyMaterial()->SetVectorParameterValue("SkyColorMultiply", SkyColorMultiply);
	GetSkyMaterial()->SetScalarParameterValue("Intensity", GetSkyLuminance() * FMath::Pow(2, Timelapse->NDFilterStops) * SkyBoxIntensity);
	GetSkyMaterial()->SetScalarParameterValue("ElevationCorrection", ElevationCorrection);
	GetSkyMaterial()->SetScalarParameterValue("Saturation", SkyColorSaturation * 0.65f);
	GetSkyMaterial()->SetScalarParameterValue("Contrast", SkyColorContrast);
	
}

void AHemisphereSky::UpdateTextures()
{
	if (HasMomentChanged())
	{
		PreloadTexture(GetMoment(-1).Motion.ToSoftObjectPath());
		PreloadTexture(GetMoment(2).Motion.ToSoftObjectPath());

		PreloadTexture(GetMoment(-1).Sky.ToSoftObjectPath());
		PreloadTexture(GetMoment(2).Sky.ToSoftObjectPath());

		GetSkyMaterial()->SetTextureParameterValue("Motion_A", DynamicGetTexture(GetCurrentMoment().Motion));
		GetSkyMaterial()->SetTextureParameterValue("Motion_B", DynamicGetTexture(GetNextMoment().Motion));

		GetSkyMaterial()->SetTextureParameterValue("Sky_A", DynamicGetTexture(GetCurrentMoment().Sky));
		GetSkyMaterial()->SetTextureParameterValue("Sky_B", DynamicGetTexture(GetNextMoment().Sky));
	}
}

bool AHemisphereSky::HasMomentChanged()
{
	if (CurrentMoment.Time != GetCurrentMoment().Time)
	{
		CurrentMoment = GetCurrentMoment();
		return true;
	}
	else
	{
		return false;
	}
}

UTexture2D* AHemisphereSky::DynamicGetTexture(TSoftObjectPtr<UTexture2D> Texture)
{
	if (Texture.IsValid())
	{
		return Texture.Get();
	}
	else
	{
		StreamableManager.RequestSyncLoad(Texture.ToSoftObjectPath());
		return Texture.Get();
	}
}

UTextureCube* AHemisphereSky::DynamicGetTextureCube(TSoftObjectPtr<UTextureCube> Texture)
{
	if (Texture.IsValid())
	{
		return Texture.Get();
	}
	else
	{
		StreamableManager.RequestSyncLoad(Texture.ToSoftObjectPath());
		return Texture.Get();
	}
}

FLinearColor AHemisphereSky::GetAverageColor()
{
	FLinearColor ColorA =   GetCurrentMoment().Average.LinearRGBToHSV();
	ColorA.R = ColorA.R + (AverageColorHueShift * 360.0f);
	ColorA.G = ColorA.G * AverageColorSaturation;
	ColorA.B = ColorA.B * AverageColorValue;
	ColorA = ColorA.HSVToLinearRGB() * SkyColorMultiply;

	FLinearColor ColorB = GetNextMoment().Average.LinearRGBToHSV();
	ColorB.R = ColorB.R + (AverageColorHueShift * 360.0f);
	ColorB.G = ColorB.G * AverageColorSaturation;
	ColorB.B = ColorB.B * AverageColorValue;
	ColorB = ColorB.HSVToLinearRGB() * SkyColorMultiply;

	return FLinearColor::LerpUsingHSV(ColorA, ColorB, GetInBetween());
}

float AHemisphereSky::GetInBetween()
{
	uint32 Selection = Timelapse->Moments.Num() - 2;
	float TimeTotal = Time * Selection;
	return FMath::Fmod(TimeTotal, 1.0f);
}

void AHemisphereSky::UpdateFog()
{
	if (FogUpdateSettings.SetDirectionalInscatteringColor)
	{
		ExponentialHeightFog->SetDirectionalInscatteringColor(FLinearColor(1.0,0.75,0.5,1.0) * DirectionalInscatter / 10.f);
	}
	
	if (FogUpdateSettings.SetFogInscatteringColor)
	{
		ExponentialHeightFog->SetFogInscatteringColor(GetAverageColor() * Inscatter * (GetSkyLuminance() / FMath::Pow(2,Timelapse->NDFilterStops)));
	}
}

float AHemisphereSky::GetLuminance(float exposure)
{
	// Using https://en.wikipedia.org/wiki/Exposure_value
	float L = FMath::Pow(2, exposure - 3);
	return L;
}

float AHemisphereSky::GetIlluminance(float exposure)
{
	// Using https://en.wikipedia.org/wiki/Exposure_value
	float E = FMath::Pow(2, exposure) * 2.5;
	return E;
}

UHemisphereTimelapse* AHemisphereSky::GetDefaultTimelapse()
{
	UHemisphereTimelapse* FoundTimelapse = nullptr;

	UClass* AssetClass = UHemisphereTimelapse::StaticClass();
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	const UClass* Class = UStaticMesh::StaticClass();
	AssetRegistryModule.Get().GetAssetsByClass(AssetClass->GetFName(), AssetData);

	if (AssetData.Num() > 0)
	{
		FoundTimelapse = Cast<UHemisphereTimelapse>(AssetData[0].GetAsset());
	}

	return FoundTimelapse;
}

FString AHemisphereSky::AddLeadingZeroes(const uint32& Number, const int Length)
{
	char Buffer[100];
	int RespCode;
	RespCode = snprintf(Buffer, 100, "%0*d", Length, Number);
	return FString(ANSI_TO_TCHAR(Buffer));
}
