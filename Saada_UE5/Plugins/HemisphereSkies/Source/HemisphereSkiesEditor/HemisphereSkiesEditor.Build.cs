// Copyright 2020 Wouter Weynants. All Rights Reserved.

using UnrealBuildTool;

public class HemisphereSkiesEditor : ModuleRules
{
	public HemisphereSkiesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "AssetTools",
                "RenderCore",
                "HemisphereSkies",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
                "Slate",
				"SlateCore",
                "Settings",
            }
			);
	}
}
