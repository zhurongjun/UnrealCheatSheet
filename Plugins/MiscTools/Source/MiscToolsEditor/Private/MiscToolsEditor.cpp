#include "MiscToolsEditor.h"
#include "LevelEditor.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "FMiscToolsEditorModule"
#define MODULE_NAME "MiscToolsEditor"

void FMiscToolsEditorModule::InitStyle()
{
	// create slate style
	m_Style = MakeShareable(new FSlateStyleSet(MODULE_NAME"Style"));
	m_Style->SetContentRoot(IPluginManager::Get().FindPlugin("MiscTools")->GetBaseDir() / TEXT("Resources"));
	m_Style->Set(MODULE_NAME".OpenPluginWindow",
        new FSlateImageBrush(
            m_Style->RootToContentDir(TEXT("Icon128"),
                TEXT(".png")),
                FVector2D(40,40)));
	
	// register style
	FSlateStyleRegistry::RegisterSlateStyle(*m_Style);

	// reload texture
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

void FMiscToolsEditorModule::ShutDownStyle()
{
	// unregister style 
	FSlateStyleRegistry::UnRegisterSlateStyle(*m_Style);

	// destroy style 
	ensure(m_Style.IsUnique());
	m_Style.Reset();
}

void FMiscToolsEditorModule::InitCommand()
{
	// register commands
	FMiscToolsEditorCommand::Register();
	
	// create commands  
	m_Commands = MakeShareable(new FUICommandList);

	// register commands
	m_Commands->MapAction(
		FMiscToolsEditorCommand::Get().OpenPluginWindow ,
		FExecuteAction::CreateLambda([]{ FGlobalTabmanager::Get()->InvokeTab(FName(TEXT(MODULE_NAME))); }) ,
		FCanExecuteAction());
}

void FMiscToolsEditorModule::ShutDownCommand()
{
	FMiscToolsEditorCommand::Unregister();
}

void FMiscToolsEditorModule::InitEditorWidgets()
{
	// Get level editor module 
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// add menu button 
	{
		auto ExtendMenu = [](FMenuBuilder& InBuilder)
		{
			InBuilder.AddMenuEntry(FMiscToolsEditorCommand::Get().OpenPluginWindow);
		};
		
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension(
			"WindowLayout",
			EExtensionHook::After,
			m_Commands,
			FMenuExtensionDelegate::CreateLambda(ExtendMenu));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	// add add tool bar button
	{
		auto ExtendToolBar = [](FToolBarBuilder& InBuilder)
		{
			InBuilder.AddToolBarButton(FMiscToolsEditorCommand::Get().OpenPluginWindow);
		};
		
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension(
			"Settings",
			EExtensionHook::After,
			m_Commands,
			 FToolBarExtensionDelegate::CreateLambda(ExtendToolBar));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	// register tab spawner
	{
		auto SpawnTab = [](const FSpawnTabArgs& SpawnTabArgs)
		{
			// here just spawn a table with text
			return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),40))
					.ColorAndOpacity(FSlateColor(FLinearColor::Blue))
					.Text(FText::FromString(TEXT("Oh hhhhhhhhhhhhhh")))
				]
			];
		};
		
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			TEXT(MODULE_NAME), FOnSpawnTab::CreateLambda(SpawnTab))
	        .SetDisplayName(LOCTEXT(MODULE_NAME"Title", MODULE_NAME))
	        .SetMenuType(ETabSpawnerMenuType::Hidden);
	}
}

void FMiscToolsEditorModule::ShutDownEditorWidgets()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TEXT(MODULE_NAME));
}

void FMiscToolsEditorModule::StartupModule()
{
	InitStyle();
	InitCommand();
	InitEditorWidgets();
}

void FMiscToolsEditorModule::ShutdownModule()
{
	ShutDownStyle();
	ShutDownCommand();
	ShutDownEditorWidgets();
}

void FMiscToolsEditorCommand::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, MODULE_NAME, "Bring up MiscToolsEditor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef MODULE_NAME
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscToolsEditorModule, MiscToolsEditor)