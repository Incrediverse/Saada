// Copyright 2020 Wouter Weynants. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "HemisphereTimelapseFactory.generated.h"

UCLASS()
class UHemisphereTimelapseFactory : public UFactory
{
	GENERATED_BODY()

public:

	UHemisphereTimelapseFactory();

	virtual uint32 GetMenuCategories() const override;

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
