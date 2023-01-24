// Copyright 2020 Wouter Weynants. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "HemisphereSkiesStyle.h"

class FHemisphereSkiesCommands : public TCommands<FHemisphereSkiesCommands>
{
public:

	FHemisphereSkiesCommands()
		: TCommands<FHemisphereSkiesCommands>(TEXT("HemisphereSkies"), NSLOCTEXT("Contexts", "HemisphereSkies", "HemisphereSkies Plugin"), NAME_None, FHemisphereSkiesStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
