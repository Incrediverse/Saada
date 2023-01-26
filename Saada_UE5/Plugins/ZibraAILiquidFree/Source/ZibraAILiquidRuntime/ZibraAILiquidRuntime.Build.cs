// Copyright Zibra AI, Inc 2022. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ZibraAILiquidRuntime : ModuleRules
{
    private const string ReleaseLibDir = "Release";
    private const string DynamicLibName = "ZibraFluidNative_Win.dll";

    public ZibraAILiquidRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        OptimizeCode = CodeOptimization.Never;

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/ZibraAILiquidRuntimePCH.h"));

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "RenderCore",
                "RHI",
				"Json",
				"JsonUtilities",
				"Slate",
				"SlateCore",

			}
        );

#if UE_5_0_OR_LATER
	PublicDependencyModuleNames.Add("HTTP");
#else 
	PublicDependencyModuleNames.Add("Http");
#endif

		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"CoreUObject",
                "Projects",
				"Renderer", 
				"RenderCore", 
				"RHI"
			}
        );

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"UnrealEd"
				}
			);
		}

		if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateDependencyModuleNames.Add("D3D11RHI");
			PublicDefinitions.Add("DX11_SUPPORT=1");
#if UE_5_0_OR_LATER
			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
			PrivateDependencyModuleNames.Add("D3D12RHI");
			PrivateDependencyModuleNames.Add("RHICore");
			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			PrivateIncludePaths.AddRange(
				new string[]{
						//required for FD3D12GPUFence
						Path.Combine(EngineDir, "Source/Runtime/D3D12RHI/Private"),
						Path.Combine(EngineDir, "Source/Runtime/D3D12RHI/Private/Windows")
				});
			PublicDefinitions.Add("DX12_SUPPORT=1");
#else
			PublicDefinitions.Add("DX12_SUPPORT=0");	
#endif
		}

        string ZibraNativeBinLocation = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ZibraFluidNative"));

        bool bHasSupport = File.Exists(Path.Combine(ZibraNativeBinLocation, ReleaseLibDir, DynamicLibName));

        if (bHasSupport)
        {
            PublicDefinitions.Add("ZIBRA_AI_NATIVE_SUPPORT=1");

            PublicIncludePaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "Public")));

            PrivateIncludePaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "Private")));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add(Path.Combine(DynamicLibName));
            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add(Path.Combine(ZibraNativeBinLocation, ReleaseLibDir, DynamicLibName));
        }
        else
        {
            PublicDefinitions.Add("ZIBRA_AI_NATIVE_SUPPORT=0");
        }
    }
}