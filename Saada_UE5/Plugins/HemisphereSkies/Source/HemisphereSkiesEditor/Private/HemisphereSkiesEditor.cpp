// Copyright 2020 Wouter Weynants. All Rights Reserved.

#include "HemisphereSkiesEditor.h"
#include "HemisphereSkiesStyle.h"
#include "HemisphereSkiesCommands.h"
#include "Misc/MessageDialog.h"
#include "HemisphereSkies/Public/HemisphereSky.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISettingsModule.h"
#include "HemisphereSettings.h"
#include "LevelEditor.h"

static const FName HemisphereSkiesEditorTabName("HemisphereSkiesEditor");

#define LOCTEXT_NAMESPACE "FHemisphereSkiesEditorModule"

void FHemisphereSkiesEditorModule::StartupModule()
{
	FHemisphereSkiesStyle::Initialize();
	FHemisphereSkiesStyle::ReloadTextures();

	FHemisphereSkiesCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FHemisphereSkiesCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FHemisphereSkiesEditorModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FHemisphereSkiesEditorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FHemisphereSkiesEditorModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Hemisphere",
			LOCTEXT("RuntimeSettingsName", "Hemisphere"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Hemisphere's Settings"),
			GetMutableDefault<UHemisphereSettings>());
	}
}

void FHemisphereSkiesEditorModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Hemisphere");
	}

	FHemisphereSkiesStyle::Shutdown();

	FHemisphereSkiesCommands::Unregister();
}

void FHemisphereSkiesEditorModule::PluginButtonClicked()
{
	UWorld* World = GEngine->GetWorldContexts()[0].World();
	AHemisphereSky* FoundHemisphere = Cast<AHemisphereSky>(UGameplayStatics::GetActorOfClass(World, AHemisphereSky::StaticClass()));
	
	if (FoundHemisphere)
	{
		UE_LOG(LogTemp, Log, TEXT("Selected Hemisphere named: %s"), *FoundHemisphere->GetName());
		GEditor->SelectNone(false, true);
		GEditor->SelectActor(FoundHemisphere, true, true);
	}
	else
	{
		FText DialogText = FText::Format(
			LOCTEXT("PluginButtonDialogText", "No {0} was found in the scene, would you like to add one?"),
			FText::FromString(TEXT("HemisphereSkyActor"))
		);
		EAppReturnType::Type DialogType = FMessageDialog::Open(EAppMsgType::YesNo, DialogText);
		if (DialogType == EAppReturnType::Yes)
		{
			FTransform SpawnTransform;
			FoundHemisphere = Cast<AHemisphereSky>(UGameplayStatics::BeginDeferredActorSpawnFromClass(World, AHemisphereSky::StaticClass(), SpawnTransform));
			UGameplayStatics::FinishSpawningActor(FoundHemisphere, SpawnTransform);
			if (FoundHemisphere->Timelapse == nullptr)
			{
				FText NoTimelapseText = FText::Format(
					LOCTEXT("PluginButtonDialogText", "No Timelapses were found in the content browser. Please add at least one Timelapse to your project in order to use Hemisphere. Visit hemisphere-realtime.xyz for more info"),
					FText::FromString(TEXT("HemisphereSkyActor"))
				);
				FMessageDialog::Open(EAppMsgType::Ok, NoTimelapseText);
				FoundHemisphere->Destroy();
			}
			else
			{
				GEditor->SelectNone(false, true);
				GEditor->SelectActor(FoundHemisphere, true, true);
			}
		}
	}
}

void FHemisphereSkiesEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FHemisphereSkiesCommands::Get().PluginAction);
}

void FHemisphereSkiesEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FHemisphereSkiesCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHemisphereSkiesEditorModule, HemisphereSkiesEditor)