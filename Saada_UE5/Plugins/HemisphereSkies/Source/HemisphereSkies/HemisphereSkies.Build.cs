// Copyright 2020 Wouter Weynants. All Rights Reserved.

using UnrealBuildTool;

public class HemisphereSkies : ModuleRules
{
	public HemisphereSkies(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("HemisphereSkies/Private");

        PrecompileForTargets = PrecompileTargetsType.Any;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
            }
			);
    }
}
