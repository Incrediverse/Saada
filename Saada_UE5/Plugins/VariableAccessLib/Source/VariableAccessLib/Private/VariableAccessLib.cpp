// Copyright © Pixeltale Motion 2021. All rights reserved.

#include "VariableAccessLib.h"

#define LOCTEXT_NAMESPACE "FVariableAccessLibModule"

void FVariableAccessLibModule::StartupModule()
{
	/*// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowPages = true;
	InitOptions.bAllowClear = true;
	InitOptions.bShowFilters = true;
	MessageLogModule.RegisterLogListing("VariableAccess", NSLOCTEXT("VariableAccess", "VariableAccessLogLabel", "Variable Access"), InitOptions);*/
}

void FVariableAccessLibModule::ShutdownModule()
{
	/*// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (FModuleManager::Get().IsModuleLoaded("MessageLog"))
	{
		// unregister message log
		FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.UnregisterLogListing("VariableAccess");
	}*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVariableAccessLibModule, VariableAccessLib)