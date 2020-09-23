// Fill out your copyright notice in the Description page of Project Settings.


#include "FukoAssetTools.h"
#include "AssetRegistryModule.h"
#include "FileHelpers.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Engine/AssetManager.h"
#include "Engine/ObjectLibrary.h"

UObject* UFukoAssetTools::CreateAsset(UClass* InClass, const FString& InName, const FString& InPath)
{
	// validate InClass
	if (InClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Asset Failed, Reason: InClass is null"));
		return nullptr;
	}

	// validate path
	FString PackageName = InPath;
	{
		FText FailedReason;
		if (FPaths::ValidatePath(PackageName, &FailedReason) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Create Asset Failed, Reason: %s"), *FailedReason.ToString());
			return nullptr;
		}

		// remove space 
		while (PackageName.EndsWith(" ")) PackageName.RemoveAt(PackageName.Len() - 1);

		// validate path
		if (!PackageName.EndsWith("/") && !PackageName.EndsWith("\\"))
		{
			UE_LOG(LogTemp, Error, TEXT("Create Asset Failed, Reason: Path must end with / or \\"));
			return nullptr;
		}
	}

	// create package
	PackageName.Append(InName);
	UPackage* Package = CreatePackage(nullptr, *PackageName);

	// create object 
	UObject* NewObj = NewObject<UObject>(Package, InClass, *InName, RF_Standalone | RF_Public);

	// register object 
	NewObj->PostEditChange();
	NewObj->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewObj);

	// save object
	TArray<UPackage*> PackageToSave;
	PackageToSave.Add(Package);
	UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);

	return NewObj;
}

UObject* UFukoAssetTools::CreateAssetWitDialog(UClass* InClass, bool AllowOverlapExistAsset)
{
	// validate 
	if (InClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Asset Failed, Reason: InClass is null"));
		return nullptr;
	}

	// fill config 
	FSaveAssetDialogConfig Config;
	Config.DefaultAssetName = TEXT("New");
	Config.DefaultAssetName.Append(InClass->GetName());
	Config.ExistingAssetPolicy = AllowOverlapExistAsset ?
		ESaveAssetDialogExistingAssetPolicy::Disallow :
		ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
	Config.DefaultPath = TEXT("/Game/");
	Config.AssetClassNames.Add(InClass->GetFName());

	// open content browser 
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(Config);

	// cancel
	if (SaveObjectPath.IsEmpty()) return nullptr;

	// split asset name and asset package 
	FString AssetName;
	FString AssetPackage;
	SaveObjectPath.Split(".", &AssetPackage, &AssetName);

	// create package
	UPackage* Package = CreatePackage(nullptr, *AssetPackage);

	// create object 
	UObject* NewObj = NewObject<UObject>(Package, InClass, *AssetName, RF_Standalone | RF_Public);

	// register object 
	NewObj->PostEditChange();
	NewObj->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewObj);

	// save object
	TArray<UPackage*> PackageToSave;
	PackageToSave.Add(Package);
	UEditorLoadingAndSavingUtils::SavePackages(PackageToSave, true);

	return NewObj;
}

bool UFukoAssetTools::SaveAsset(UObject* InObject)
{
	// validate 
	if (InObject == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Save Asset Failed, Reason: InObject if null"));
		return false;
	}
	UObject* Outer = InObject->GetOuter();
	UPackage* Pkg = Cast<UPackage>(Outer);
	if (Pkg == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Save Asset Failed, Reason: InObject outer is null or outer not a UPackage"));
		return false;
	}

	// mark dirty 
	InObject->PostEditChange();
	InObject->MarkPackageDirty();

	// save asset 
	TArray<UPackage*> PackagesToSave({Pkg});
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, true);
	return true;
}

bool UFukoAssetTools::SaveAssets(const TArray<UObject*>& InObjects)
{
	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Reserve(InObjects.Num());

	// fill packages 
	for (int32 i = 0; i < InObjects.Num(); ++i)
	{
		UObject* Obj = InObjects[i];
		// validate 
		if (Obj == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Save Asset Failed, Reason: Index %d Object if null"), i);
			return false;
		}
		UObject* Outer = Obj->GetOuter();
		UPackage* Pkg = Cast<UPackage>(Outer);
		if (Pkg == nullptr)
		{
			UE_LOG(LogTemp, Error,
				   TEXT("Save Asset Failed, Reason: Index %d Object outer is null or outer not a UPackage"), i);
			return false;
		}

		// mark dirty 
		Obj->PostEditChange();
		Obj->MarkPackageDirty();

		PackagesToSave.Add(Pkg);
	}

	// save asset 
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, true);
	return true;
}


bool UFukoAssetTools::DeleteAsset(UObject* InObject)
{
	TArray<FAssetData> AssetToDelete;
	AssetToDelete.Emplace(InObject, true);
	ObjectTools::DeleteAssets(AssetToDelete);
	return true;
}

bool UFukoAssetTools::DeleteAssets(const TArray<UObject*>& InObjects)
{
	TArray<FAssetData> AssetToDelete;
	for (int32 i = 0; i < InObjects.Num(); ++i)
	{
		UObject* Obj = InObjects[i];
		AssetToDelete.Emplace(Obj, true);
	}
	ObjectTools::DeleteAssets(AssetToDelete);
	return true;
}

UObject* UFukoAssetTools::LoadAsset(const FString& InPath, const FString& InName)
{
	FString FullPath = InPath;
	FullPath.PathAppend(*InName,InName.Len());
	FullPath.AppendChar(TEXT('.'));
	FullPath.Append(InName);
	return StaticLoadObject(UObject::StaticClass(),nullptr, *FullPath);
}

TArray<UObject*> UFukoAssetTools::LoadAssetsInPath(const FString& InPath, bool bIncludeBlueprint, UClass* InClass)
{
	TArray<UObject*>  RetObjects;
	
	// create library 
	UObjectLibrary* Library = UObjectLibrary::CreateLibrary(InClass, bIncludeBlueprint, GIsEditor);
	if (Library == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Load Assets Failed, Reason: Faild to create library"));
		return RetObjects;
	}
	Library->AddToRoot();	// Prevent gc
	
	// load assets
	FString SearchPath = InPath;
	SearchPath.RemoveFromEnd("/"); 
	Library->LoadAssetDataFromPath(SearchPath);
	Library->LoadAssetsFromAssetData();

	// get asset
	TArray<FAssetData> AssetsData;
	Library->GetAssetDataList(AssetsData);

	// fill return array
	for(int32 i = 0; i < AssetsData.Num(); ++i)
	{
		RetObjects.Add(AssetsData[i].GetAsset());
	}
	
	Library->RemoveFromRoot();	// Allow gc
	return RetObjects;
}

TArray<FAssetData> UFukoAssetTools::GetAssetsDataInPath(const FString& InPath, bool bIncludeBlueprint, UClass* InClass)
{
	TArray<FAssetData>  RetData;
	
	// create library 
	UObjectLibrary* Library = UObjectLibrary::CreateLibrary(InClass, bIncludeBlueprint, GIsEditor);
	if (Library == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Load Assets Failed, Reason: Faild to create library"));
		return RetData;
	}
	Library->AddToRoot();	// Prevent gc
	
	// load assets
	FString SearchPath = InPath;
	SearchPath.RemoveFromEnd("/"); 
	Library->LoadAssetDataFromPath(SearchPath);

	// get asset
	Library->GetAssetDataList(RetData);
	
	Library->RemoveFromRoot();	// Allow gc
	return RetData;
	
}

