#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DialogTools.generated.h"

/**
 * Tools for open system dialog 
 */
UCLASS()
class MISCTOOLS_API UDialogTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//========================open system dialog========================
	UFUNCTION(BlueprintCallable)
        static FString OpenFileDialog(
            const FString& InTitle = TEXT("SelectFile") ,
            const FString& InDefaultPath = TEXT(""),
            const FString& InDefaultFile = TEXT(""),
            const FString& InFileType = TEXT(""));

	UFUNCTION(BlueprintCallable)
    static void OpenFileDialogEx(
    	TArray<FString>& OutArray,
    	bool bAllowMutiSelect = false,
        const FString& InTitle = TEXT("SelectFile"),
        const FString& InDefaultPath = TEXT(""),
        const FString& InDefaultFile = TEXT(""),
        const FString& InFileType = TEXT(""));

	UFUNCTION(BlueprintCallable)
    static FString OpenDictionaryDialog(
       const FString& InTitle = TEXT("SelectDictionary") ,
       const FString& InDefaultPath = TEXT(""));
	//=====================end open system dialog========================

	//========================open unreal dialog=========================
	UFUNCTION(BlueprintCallable)
	static void OpenSaveAssetDialog(
        FString& OutName ,
        FString& OutPackageName,
        const TArray<UClass*>& InAssetClasses,
		const FString& InTitle = TEXT("CreateAsset") ,
        const FString& InDefaultName = TEXT("") ,
        const FString& InDefaultPath = TEXT("/Game/") ,
		bool AllowOverlapExistAsset = false);
	//======================end open unreal dialog=======================
};
