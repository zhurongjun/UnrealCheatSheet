// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MiscTools : ModuleRules
{
	public MiscTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		 
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"RenderCore" ,		// for register shader path 
				"RHI" ,				// for use RHI api 
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Slate",
				"SlateCore",
				
				"DesktopPlatform",	// for open system dialog 
				"AssetRegistry",	// for asset operator 
				"AssetTools",		// for asset operator 
				"UnrealEd",			// for use editor function 
				"ContentBrowser" ,	// for open engine dialog 
				"Projects" ,		// for use IPluginManager 
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}