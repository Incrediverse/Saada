// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "VariableAccessSettings.generated.h"

UCLASS(config = VariableAccess, defaultconfig, meta = (DisplayName = "Variable Access"))
class VARIABLEACCESSLIB_API UVariableAccessSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	
	UPROPERTY(config, EditAnywhere, Category = VariableAccess)
		bool bPrintErrorsToLog = false;
	UPROPERTY(config, EditAnywhere, Category = VariableAccess)
		bool bPrintErrorsToScreen = false;

	// Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
	// END UDeveloperSettings Interface

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};