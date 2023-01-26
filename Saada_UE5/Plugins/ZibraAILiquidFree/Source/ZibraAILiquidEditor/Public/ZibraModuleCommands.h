// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ZibraModuleStyle.h"

class FZibraModuleCommands : public TCommands<FZibraModuleCommands>
{
public:

	FZibraModuleCommands()
		: TCommands<FZibraModuleCommands>(TEXT("ZibraModule"), NSLOCTEXT("Contexts", "ZibraModule", "ZibraModule Plugin"), NAME_None, FZibraModuleStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};