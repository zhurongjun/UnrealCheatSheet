#include "MiscToolsEditor.h"
#include "FukoAssetTools.h"
#include "LevelEditor.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "UI/SCategoryTab.h"
#include "WidgetBlueprint.h"

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
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			TEXT(MODULE_NAME), FOnSpawnTab::CreateRaw(this, &FMiscToolsEditorModule::OnSpawnPluginTab))
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
	CleanPages();
}

TSharedRef<SDockTab> FMiscToolsEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// collect pages
	CleanPages();
	CollectPages();
	
	// Create categories 
	auto RetDockTab = SNew(SDockTab)
    .TabRole(ETabRole::NomadTab)
    [
        SNew(SBorder)
        .Padding(FMargin(3))
        .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            [
                // LeftTab 
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                  .HAlign(HAlign_Left)
                  .VAlign(VAlign_Fill)
                  .AutoWidth()
                [
                    SAssignNew(m_CategoryTab,SCategoryTab)
                    .OnCategoryChanged_Raw(this, &FMiscToolsEditorModule::OnCategoryChanged)
                ]
                // RightPanel 
                + SHorizontalBox::Slot()
                  .HAlign(HAlign_Fill)
                  .VAlign(VAlign_Fill)
                [
                    SNew(SBorder)
                .Padding(3)
                .BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
                    [
                        SAssignNew(m_RightPanel,SBox)
                        .Padding(3)
                    ]
                ]
            ]
        ]
    ];

	// add pages
	FName DefaultCategoryName = NAME_None;
	for (UMiscToolsPage* Page : m_AllPages)
	{
		if (Page->GetPageName() != NAME_None)
		{
			if (DefaultCategoryName == NAME_None) DefaultCategoryName = Page->GetPageName();
			m_CategoryTab->AddCategory(Page->GetPageName());
		}
	}
	for (UMiscToolsPageBlueprint* Page : m_AllBlueprintPages)
	{
		if (DefaultCategoryName == NAME_None) DefaultCategoryName = Page->GetFName();
		m_CategoryTab->AddCategory(Page->GetFName());
	}

	if (DefaultCategoryName != NAME_None)
	{
		m_CategoryTab->SetCurrentCategory(DefaultCategoryName);
	}
	
	return RetDockTab;
}

void FMiscToolsEditorModule::OnCategoryChanged(FName Category)
{
	// find page 
	UMiscToolsPage** Page = m_AllPages.FindByPredicate(
        [=](UMiscToolsPage* InPage)->bool
        {
            return InPage->GetPageName() == Category;
        });
	UMiscToolsPageBlueprint** BPPage = m_AllBlueprintPages.FindByPredicate(
		[=](UMiscToolsPageBlueprint* InPage)
		{
			return InPage->GetFName() == Category;
		});

	// set content 
	if (Page)
	{
		m_RightPanel->SetContent((*Page)->GetPageContent());
	}
	else if (BPPage)
	{
		m_RightPanel->SetContent((*BPPage)->TakeWidget());
	}
}

void FMiscToolsEditorModule::CleanPages()
{
	for (UMiscToolsPage* Page : m_AllPages)
	{
		Page->RemoveFromRoot();
	}
	for (UMiscToolsPageBlueprint* Page : m_AllBlueprintPages)
	{
		Page->RemoveFromRoot();
	}
	m_AllPages.Reset();
	m_AllBlueprintPages.Reset();
}

void FMiscToolsEditorModule::CollectPages()
{
	// collect cpp pages 
	TArray<UClass*>	AllToolPageClasses;
	GetDerivedClasses(UMiscToolsPage::StaticClass(), AllToolPageClasses, true);
	for (UClass* ToolPage : AllToolPageClasses)
	{
		if (ToolPage->ClassFlags & EClassFlags::CLASS_Abstract) continue;
		UMiscToolsPage* DefaultObj = Cast<UMiscToolsPage>(ToolPage->GetDefaultObject());
		DefaultObj->AddToRoot();
		m_AllPages.Add(DefaultObj);
	}

	// collect blueprint pages
 	TArray<UObject*> AllAssets = UFukoAssetTools::LoadAssetsInPath(TEXT("/MiscTools/"), true, UWidgetBlueprint::StaticClass());
	for (UObject* Asset : AllAssets)
	{
		UWidgetBlueprint* BPAsset = Cast<UWidgetBlueprint>(Asset);
		TSharedRef<SWidget> TabWidget = SNullWidget::NullWidget;
		TSubclassOf<UUserWidget> WidgetClass(BPAsset->GeneratedClass);
		UWorld* World = GEditor->GetEditorWorldContext().World();
		UMiscToolsPageBlueprint* NewWidget = CreateWidget<UMiscToolsPageBlueprint>(World, WidgetClass, BPAsset->GetFName());
		if (NewWidget == nullptr) continue;
		NewWidget->AddToRoot();
		m_AllBlueprintPages.Add(NewWidget);
	}
}

void FMiscToolsEditorCommand::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, MODULE_NAME, "Bring up MiscToolsEditor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef MODULE_NAME
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMiscToolsEditorModule, MiscToolsEditor)