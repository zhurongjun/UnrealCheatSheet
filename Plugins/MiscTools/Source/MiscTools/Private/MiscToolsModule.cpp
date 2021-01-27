#include "CoreMinimal.h"

#include "CrashTools.h"
#include "LevelEditor.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Modules/ModuleManager.h"


#define LOCTEXT_NAMESPACE "FMiscToolsModule"

class FSlateStyleSet;
class FLevelEditorModule;

class FMiscToolsModule : public IModuleInterface
{
public:
	// ~Begin IModuleInterface 
	virtual void StartupModule() override
	{
		// register shader dictionary 
		FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MiscTools"))->GetBaseDir(),TEXT("Shaders"));
		AddShaderSourceDirectoryMapping(TEXT("/Plugin/MiscTools"), ShaderDir);

		// change Error handler
		static FCrashTools CrashTool(GError);
		GError = &CrashTool;
	}
	
	virtual void ShutdownModule() override
	{
		
	}
	// ~End IModuleInterface
};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscToolsModule, MiscTools)
