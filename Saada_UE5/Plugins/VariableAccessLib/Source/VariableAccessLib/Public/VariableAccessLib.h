// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "MessageLog/Public/MessageLogInitializationOptions.h"
#include "MessageLog/Public/MessageLogModule.h"

class FVariableAccessLibModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
