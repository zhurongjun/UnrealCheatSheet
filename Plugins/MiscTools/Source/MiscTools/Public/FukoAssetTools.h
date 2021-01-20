// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FukoAssetTools.generated.h"

/**
 * tools for asset generate, delete......
 *
 * unreal asset tool, get IAssetTools  
 *     	GetAssetTools: UAssetToolsHelpers::GetAssetTools()
 * about unreal pak
 * 		https://github.com/jashking/UnrealPakViewer
 * 		https://github.com/hxhb/HotPatcher
 *  for blueprint file operator
 *  	use Built-in plugin BluePrint File Utilities
 */
UCLASS()
class MISCTOOLS_API UFukoAssetTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UObject* CreateAsset(UClass* InClass, const FString& InName, const FString& InPath);
	UFUNCTION(BlueprintCallable)
	static UObject* CreateAssetWitDialog(UClass* InClass, bool AllowOverlapExistAsset = false);

	UFUNCTION(BlueprintCallable)
	static bool SaveAsset(UObject* InObject);
	UFUNCTION(BlueprintCallable)
	static bool SaveAssets(const TArray<UObject*>& InObjects);
	
	UFUNCTION(BlueprintCallable)
	static bool DeleteAsset(UObject* InObject);
	UFUNCTION(BlueprintCallable)
	static bool DeleteAssets(const TArray<UObject*>& InObjects);

	UFUNCTION(BlueprintCallable)
	static UObject* LoadAsset(const FString& InPath, const FString& InName);

	UFUNCTION(BlueprintCallable)
	static TArray<UObject*> LoadAssetsInPath(const FString& InPath, bool bIncludeBlueprint = true ,UClass* InClass = nullptr);

	UFUNCTION(BlueprintCallable)
	static TArray<FAssetData> GetAssetsDataInPath(const FString& InPath, bool bIncludeBlueprint = true ,UClass* InClass = nullptr);

	UFUNCTION(BlueprintCallable)
	static void OpenAssetEditor(UObject* Asset);

	UFUNCTION(BlueprintCallable)
    static TArray<UClass*> GetAllClassFromBlueprintAsset(const FString& InPath);
};
