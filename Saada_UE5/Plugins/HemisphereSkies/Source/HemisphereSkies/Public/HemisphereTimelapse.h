// Copyright 2020 Wouter Weynants. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Engine/Texture.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "HemisphereTimelapse.generated.h"

/** Sun Data */
USTRUCT(BlueprintType)
struct HEMISPHERESKIES_API FHemisphereSunPositionData
{
	GENERATED_USTRUCT_BODY()

public:

	/** Sun Elevation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	float Elevation;

	/** Sun Elevation, corrected for atmospheric diffraction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	float CorrectedElevation;

	/** Sun azimuth */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	float Azimuth;

	/** Sunrise time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	FTimespan SunriseTime;

	/** Sunset time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	FTimespan SunsetTime;

	/** Solar noon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Position")
	FTimespan SolarNoon;
};

UENUM(BlueprintType)
enum class ERawExtension : uint8
{
	RW2,
	ARW,
};

USTRUCT(BlueprintType)
struct HEMISPHERESKIES_API FHemisphereMoment : public FTableRowBase
{
	GENERATED_BODY()

	/** Moment's real life date and time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	FDateTime Time;

	/** HDR texture of the sky's upper hemisphere */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	TSoftObjectPtr<UTexture2D> Sky;

	/**  HDR motion vector texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	TSoftObjectPtr<UTexture2D> Motion;

	/** HDR Cubemap texture of the sky */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	TSoftObjectPtr<UTextureCube> Cubemap;

	/** Average Color of the sky */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	FLinearColor Average;

	/** Logged Sun Intensity in W/m2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	float SolarIlluminance;

	/** Exposure value of the recorded HDR photograph */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hemisphere Moment Struct")
	float EV;
};

UCLASS(BlueprintType)
class HEMISPHERESKIES_API UHemisphereTimelapse : public UObject
{
	GENERATED_BODY()

public:
	/** Summary of this Timelapse */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FString Description;

	/** Array containing the per-HDRI Timelapse data */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Data")
	TArray<FHemisphereMoment> Moments;

	/**
	* The version this timelapse is intended for.
	* If this does not match the current plugin version, please re-download the Timelapse asset.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	FString PluginVersion;

	/** Directory with list of RW2 exposures, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	FString SourceDirectoryOnDisk = "C:/Hemisphere/Import/000";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	ERawExtension Extension = ERawExtension::RW2;

	/** How many exposures per HDRI, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	int ExposuresPerHDRI = 7;

	/** Available exposures, selected ones will be processed, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	TArray<bool> ProcessedExposures;

	/** Aperture the pictures were taken with, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	float Aperture = 16.f;

	/** Stops of the internal ND filter applied to the camera, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import")
	float NDFilterStops = 2.f;

	/** Directory to store temporary files, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	FString ExportDirectoryOnDisk = "C:/Hemisphere/Export/000";

	/** Number of Timelapse, will determine name by Timelapse_XXX. Timelapse_000 for example with the default setting of 0, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int TimelapseNumber = 0;

	/** Directory to store temporary files, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	FString ContentBrowserLocation = "/Game/Hemisphere/CreatorTimelapses";

	/** Latitude of the recording location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	float Latitude = 51.f;

	/** Longitude of the recording location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	float Longitude = 3.7f;

	/** TimeZone of the recording location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	float TimeZone = 1.f;

	/** DST of the recording location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	bool bIsDaylightSavingTime = true;

	/** Camera rotation offset, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alignment")
	float Rotation = 0.f;

	/** Camera rotation offset, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alignment")
	float XOffset = 0.f;

	/** Camera rotation offset, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alignment")
	float YOffset = 0.f;

	/** Camera rotation offset, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Alignment")
	float FieldOfView = 203.f;

	/** Sky bake Resolution, only used during Timelapse bake using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int SkyResolution = 2048;

	/** Motion bake Resolution, only used during Timelapse bake using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int MotionResolution = 512;

	/** Cubemap bake Resolution, only used during Timelapse bake using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int CubemapResolution = 512;

	/** Offset the hour of solar data, useful if the built in clock of your solar datalogger is off, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int SolarDataHourOffset = 0;

	/** Skip X HDR photographs while baking textures to reduce interval, only used when creating new Timelapses using Hemisphere Creator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Export")
	int IntervalSkip = 0;

	bool IsPluginVersionCompatible();

	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	static void GetSunPosition(float Lat, float Long, float Zone, bool bIsDaylight, int32 Year, int32 Month, int32 Day, int32 Hours, int32 Minutes, int32 Seconds, FHemisphereSunPositionData& SunPositionData);

	UFUNCTION(BlueprintCallable, Category = "Hemisphere")
	FHemisphereSunPositionData GetSunInfo(FDateTime time);
};