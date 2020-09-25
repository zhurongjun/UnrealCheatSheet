// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealCheatSheetTarget : TargetRules
{
	public UnrealCheatSheetTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		
		// use lower build settings
		// DefaultBuildSettings = BuildSettingsVersion.V1;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		
		ExtraModuleNames.AddRange( new string[] { "UnrealCheatSheet" } );
	}
}
