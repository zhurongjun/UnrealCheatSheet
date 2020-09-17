// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiscTools.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FMiscToolsModule"

void FMiscToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MiscTools"))->GetBaseDir(),TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/MiscTools"),ShaderDir);
}

void FMiscToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscToolsModule, MiscTools)