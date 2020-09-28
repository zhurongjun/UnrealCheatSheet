#pragma once
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Modules/ModuleManager.h"

class FUICommandList;
class FSlateStyleSet;

class FMiscToolsEditorModule : public IModuleInterface
{
	// initialize and destroy process
	void InitStyle();
	void ShutDownStyle();
	void InitCommand();
	void ShutDownCommand();
	void InitEditorWidgets();
	void ShutDownEditorWidgets();

public:
	FMiscToolsEditorModule()
        : m_Style(nullptr)
	{}
	
	// ~Begin IModuleInterface 
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~End IModuleInterface

private:
	// slate style 
	TSharedPtr<FSlateStyleSet>	m_Style;

	// commands
	TSharedPtr<FUICommandList>	m_Commands;
};

// Misc tool command 
class FMiscToolsEditorCommand : public TCommands<FMiscToolsEditorCommand>
{
public:
	FMiscToolsEditorCommand()
        : TCommands<FMiscToolsEditorCommand>(
            TEXT("MiscToolsEditor"),
            NSLOCTEXT("Contexts","MiscToolsEditor","MiscToolsEditor Plugin"),
            NAME_None,
            TEXT("MiscToolsEditorStyle"))
	{}

	virtual void RegisterCommands() override;
	
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
