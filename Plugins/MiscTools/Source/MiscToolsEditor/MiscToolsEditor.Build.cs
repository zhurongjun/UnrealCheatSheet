// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MiscToolsEditor : ModuleRules
{
	public MiscToolsEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"MiscTools",
				
				"RenderCore" ,		// for register shader path 
				"RHI" ,				// for use RHI api 
				"UMG" ,				// for use UMG 
				"UMGEditor" ,		// for use UMG blueprint 
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
				"EditorStyle",		// for use editor style 
				"Blutility" ,		// for use blueprint 
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
