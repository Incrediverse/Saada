// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Containers/Ticker.h"

class FZibraAILiquidEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;

	bool Tick(float DeltaTime);

private:

	
};
