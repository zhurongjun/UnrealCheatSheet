#include "DialogTools.h"
#include "ContentBrowserModule.h"
#include "EngineUtils.h"
#include "DesktopPlatformModule.h"
#include "IContentBrowserSingleton.h"

class FContentBrowserModule;

FString UDialogTools::OpenFileDialog(const FString& InTitle, const FString& InDefaultPath, const FString& InDefaultFile,
									 const FString& InFileType)
{
	FString OutPath;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const void* ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->
														GetOSWindowHandle();

	if (ParentWindow == nullptr) return OutPath;
	if (DesktopPlatform == nullptr) return OutPath;

	TArray<FString> Paths;

	DesktopPlatform->OpenFileDialog(ParentWindow, InTitle, InDefaultPath, InDefaultFile
									, InFileType, EFileDialogFlags::None, Paths);

	if (Paths.Num() > 0) OutPath = Paths[0];
	return OutPath;
}

void UDialogTools::OpenFileDialogEx(TArray<FString>& OutArray, bool bAllowMutiSelect, const FString& InTitle,
									const FString& InDefaultPath, const FString& InDefaultFile,
									const FString& InFileType)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const void* ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->
														GetOSWindowHandle();

	if (ParentWindow == nullptr) return;
	if (DesktopPlatform == nullptr) return;

	DesktopPlatform->OpenFileDialog(ParentWindow, InTitle, InDefaultPath, InDefaultFile
									, InFileType,
									bAllowMutiSelect ? EFileDialogFlags::Multiple : EFileDialogFlags::None, OutArray);
}

FString UDialogTools::OpenDictionaryDialog(const FString& InTitle, const FString& InDefaultPath)
{
	FString OutPath;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const void* ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->
														GetOSWindowHandle();

	if (ParentWindow == nullptr) return OutPath;
	if (DesktopPlatform == nullptr) return OutPath;

	DesktopPlatform->OpenDirectoryDialog(ParentWindow, InTitle, InDefaultPath, OutPath);
	return OutPath;
}

void UDialogTools::OpenSaveAssetDialog(
        FString& OutName ,
        FString& OutPackageName,
        const TArray<UClass*>& InAssetClasses,
        const FString& InTitle ,
        const FString& InDefaultName ,
        const FString& InDefaultPath ,
        bool AllowOverlapExistAsset)
{
	// fill config 
	FSaveAssetDialogConfig Config;
	Config.DefaultAssetName = InDefaultName;
	Config.ExistingAssetPolicy = AllowOverlapExistAsset
									 ? ESaveAssetDialogExistingAssetPolicy::Disallow
									 : ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
	Config.DefaultPath = InDefaultPath;
	Config.DialogTitleOverride = FText::FromString(InTitle);
	Config.AssetClassNames.Reserve(InAssetClasses.Num());
	for (UClass* Class : InAssetClasses)
	{
		if (Class == nullptr) continue;
		Config.AssetClassNames.Add(Class->GetFName());
	}

	// clean out value
	OutName.Reset();
	OutPackageName.Reset();

	// open content browser 
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(
		"ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(Config);
	if (SaveObjectPath.IsEmpty()) return;

	// split path
	SaveObjectPath.Split(TEXT("."), &OutPackageName, &OutName);
}
