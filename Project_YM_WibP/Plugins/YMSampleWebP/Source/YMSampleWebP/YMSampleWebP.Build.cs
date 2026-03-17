// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class YMSampleWebP : ModuleRules
{
	public YMSampleWebP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"$(ModuleDir)/Public/Core",
                "$(ModuleDir)/Public/Subsystem",
                "$(ModuleDir)/Public/Type",
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "YMSampleWebP/Public/Core",
                "YMSampleWebP/Public/Log",
                "YMSampleWebP/Public/Subsystem",
                "YMSampleWebP/Public/Type",
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"YMSampleWebPLibrary",
				"Projects",
				"Engine"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
