#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiscToolsPage.generated.h"

class SWidget;

UCLASS(BlueprintType, Abstract)
class MISCTOOLSEDITOR_API UMiscToolsPage : public UObject
{
	GENERATED_BODY()
public:
	virtual TSharedRef<SWidget> GetPageContent();
	virtual FName GetPageName();
};

UCLASS()
class MISCTOOLSEDITOR_API UMiscToolsPageBlueprint : public UUserWidget
{
	GENERATED_BODY()
};


