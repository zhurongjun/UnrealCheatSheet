// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscTools.h"
#include "LevelEditor.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMiscToolsModule"

class FLevelEditorModule;

void FMiscToolsModule::StartupModule()
{
	// register shader dictionary 
	FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MiscTools"))->GetBaseDir(),TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/MiscTools"), ShaderDir);
}

void FMiscToolsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscToolsModule, MiscTools)
