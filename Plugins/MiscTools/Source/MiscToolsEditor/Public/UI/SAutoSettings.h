// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class MISCTOOLSEDITOR_API SAutoSettings : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SAutoSettings)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UObject* SettingObject);

	private:
	UObject* UISettings = nullptr;

	protected:
	TSharedPtr<IDetailsView> DetailsPanel;
};
