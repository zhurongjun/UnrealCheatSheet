// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EditorUITools.generated.h"

class UWidgetBlueprint;

/**
 * Tools for editor UI 
 */
UCLASS()
class MISCTOOLS_API UEditorUITools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// an example that open Tab Window with UEditorUtilityWidget 
	UFUNCTION(BlueprintCallable)
	static void OpenTabWindow(const FString& InTitle, UWidgetBlueprint* InContent);
	
};
