// Copyright 2020 Wouter Weynants. All Rights Reserved.

#include "HemisphereSkiesCommands.h"

#define LOCTEXT_NAMESPACE "FHemisphereSkiesModule"

void FHemisphereSkiesCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Hemisphere", "Select Hemisphere present in current map", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
