// Copyright 2020 Wouter Weynants. All Rights Reserved.


#include "HemisphereTimelapseFactory.h"
#include "HemisphereTimelapse.h"
#include "AssetTypeCategories.h"


UHemisphereTimelapseFactory::UHemisphereTimelapseFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UHemisphereTimelapse::StaticClass();
}

uint32 UHemisphereTimelapseFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Blueprint;
}

UObject* UHemisphereTimelapseFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UHemisphereTimelapse* HemisphereTimelapseAsset = NewObject<UHemisphereTimelapse>(InParent, InClass, InName, Flags);
	return HemisphereTimelapseAsset;
}

