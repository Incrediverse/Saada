// Copyright © Pixeltale Motion 2021. All rights reserved.

#include "VariableAccessSettings.h"
#include "VariableAccessLib.h"

UVariableAccessSettings::UVariableAccessSettings(const FObjectInitializer& ObjectInitlaizer)
	: Super(ObjectInitlaizer)
{

}

FName UVariableAccessSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText UVariableAccessSettings::GetSectionText() const
{
	return NSLOCTEXT("VariableAccessPlugin", "VariableAccessSettingsSection", "VariableAccess");
}
#endif

#if WITH_EDITOR
void UVariableAccessSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

