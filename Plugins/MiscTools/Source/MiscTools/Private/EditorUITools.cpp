// Fill out your copyright notice in the Description page of Project Settings.
#include "EditorUITools.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"

void UEditorUITools::OpenTabWindow(const FString& InTitle, UWidgetBlueprint* InContent)
{
	// create umg widget
	UUserWidget* NewWidget = nullptr;
	TSharedRef<SWidget> TabWidget = SNullWidget::NullWidget;
	TSubclassOf<UUserWidget> WidgetClass(InContent->GeneratedClass);
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (World)
	{
		NewWidget = CreateWidget<UUserWidget>(World, WidgetClass);
	}

	if (NewWidget == nullptr) return;
	NewWidget->AddToRoot();
	
	// create DockTab
	auto OnTabClosed = [=](TSharedRef<SDockTab> Tab)
	{
		// don't forget remove from root to destroy UMG widget 
		NewWidget->RemoveFromRoot();
	};
	
	TSharedPtr<SDockTab> NewTab = SNew(SDockTab)
		.Label(FText::FromString(InTitle))
		.TabRole(ETabRole::NomadTab)
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda(OnTabClosed))
	[
		NewWidget->TakeWidget()
	];
	
	// open window 
	FName PlaceholderId(TEXT("StandaloneToolkit"));
	TSharedPtr<FTabManager::FSearchPreference> SearchPreference = MakeShareable(new FTabManager::FRequireClosedTab());
	FGlobalTabmanager::Get()->InsertNewDocumentTab(PlaceholderId, *SearchPreference, NewTab.ToSharedRef());
}
