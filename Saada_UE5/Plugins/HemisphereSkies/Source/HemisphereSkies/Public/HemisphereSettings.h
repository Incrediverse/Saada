// Copyright 2020 Wouter Weynants. All Rights Reserved.

#pragma once

#include "HemisphereSettings.generated.h"

UCLASS(config = Hemisphere, defaultconfig)
class HEMISPHERESKIES_API UHemisphereSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = Settings)
	bool DynamicTextureLoading = false;

	UPROPERTY(EditAnywhere, config, Category = Settings, meta = (editcondition = DynamicTextureLoading))
	bool LoadAsync = true;

	UPROPERTY(EditAnywhere, config, Category = Settings, meta = (editcondition = DynamicTextureLoading))
	bool DynamicTextureUnLoading = true;

	UPROPERTY(EditAnywhere, config, Category = Settings, meta = (editcondition = DynamicTextureLoading))
	float DynamicTextureUnLoadFrequency = 0.25;

	UPROPERTY(EditAnywhere, config, Category = Info, meta = (editcondition = "false"))
	FString Version = "1.1.0";
};
