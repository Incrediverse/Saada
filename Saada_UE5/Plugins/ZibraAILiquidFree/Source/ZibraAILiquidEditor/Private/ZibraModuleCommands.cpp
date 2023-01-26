// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraModuleCommands.h"

#define LOCTEXT_NAMESPACE "FZibraModuleModule"

void FZibraModuleCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Zibra Liquids License Key", "License Key Management Window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
