// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SAutoSettings.h"

#include "SlateOptMacros.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAutoSettings::Construct(const FArguments& InArgs, UObject* SettingObject)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(
        "PropertyEditor");

	FDetailsViewArgs DetailsViewArgs(
        /*bUpdateFromSelection*/ false,
                                 /*bLockable*/ false,
                                 /*bAllowSearch*/ false,
                                 /*InNameAreaSettings*/ FDetailsViewArgs::HideNameArea,
                                 /*bHideSelectionTip*/ true,
                                 /*InNotifyHook*/ nullptr,
                                 /*InSearchInitialKeyFocus*/ false,
                                 /*InViewIdentifier*/ NAME_None);

	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bAllowMultipleTopLevelObjects = true;


	DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	if (SettingObject)
	{
		UISettings = SettingObject;
		DetailsPanel->SetObject(UISettings, true);
	}

	ChildSlot
    [
        SNew(SScrollBox)
        + SScrollBox::Slot()
        .Padding(0.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
              .AutoHeight()
              .Padding(0)
            [
                DetailsPanel.ToSharedRef()
            ]
        ]
    ];
}
