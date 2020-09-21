// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;

class FMiscToolsModule : public IModuleInterface
{
public:
	// ~Begin IModuleInterface 
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~End IModuleInterface
};