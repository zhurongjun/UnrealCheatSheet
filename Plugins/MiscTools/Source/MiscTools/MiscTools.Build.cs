// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MiscTools : ModuleRules
{
	public MiscTools(ReadOnlyTargetRules Target) : base(Target)
	{
		// even i use "CppStandard = CppStandardVersion.Cpp17"
		// PCH still use cpp14, this cause Error C2855
		// so i have to disable it 
		// may be a bug of UBT 
		PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;

		// use cpp17
		CppStandard = CppStandardVersion.Cpp17;

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
				"UMG" ,				// for use UMG function 
				"Blutility",		// for use blueprint functions
				"UMGEditor",		// Bluetility dependency 
				"Sockets" ,			// for use sockets 
				"NetWorking" ,		// for use net work 
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
