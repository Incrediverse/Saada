// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraAILiquidEditor.h"
#include "ZibraModuleStyle.h"
#include "ZibraModuleCommands.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Components/StaticMeshComponent.h"
#include "PropertyCustomizationHelpers.h"
#include "DetailLayoutBuilder.h"
#include "ToolMenus.h"
#include "UObject/Class.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"


#define LOCTEXT_NAMESPACE "FZibraAILiquidEditorModule"

static const FName ZibraLiquidsModuleTabName("ZibraLiquids");


void FZibraAILiquidEditorModule::StartupModule()
{
	
}

void FZibraAILiquidEditorModule::ShutdownModule()
{
	
}

bool FZibraAILiquidEditorModule::Tick(float DeltaTime)
{
	return true;
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZibraAILiquidEditorModule, ZibraAILiquidEditor)
