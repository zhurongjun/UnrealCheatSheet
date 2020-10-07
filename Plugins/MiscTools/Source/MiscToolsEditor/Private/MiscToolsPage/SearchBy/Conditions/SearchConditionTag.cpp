#include "MiscToolsPage/SearchBy/Conditions/SearchConditionTag.h"
#include "SourceCodeNavigation.h"
#include "Widgets/Input/SHyperlink.h"
#include "EngineUtils.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"

FName USearchConditionTag::GetConditionName()
{
	static FName ConditionName(TEXT("Tag"));
	return ConditionName;
}

TSharedRef<SWidget> USearchConditionTag::GetSearchInputWidget()
{
	return SAssignNew(InputBox, SEditableTextBox);
}

void USearchConditionTag::GetColumnSections(TArray<FName>& OutSections)
{
	OutSections += {
		FName(TEXT("ActorName")) ,
		FName(TEXT("Tag")) ,
		FName(TEXT("Class"))
	};
}

TSharedRef<SWidget> USearchConditionTag::GenColumnSection(TSharedPtr<void> InItem, FName InColumnName)
{
	Item* MyItem = (Item*)(InItem.Get());
	if (InColumnName == "ActorName")
	{
		return
            SNew(STextBlock)
            .Text(FText::FromName(MyItem->ActorName));
	}
	else if (InColumnName == "Tag")
	{
		return
            SNew(STextBlock)
            .Text(FText::FromName(MyItem->Tag));
	}
	else if (InColumnName == "Class")
	{
		UClass* ActorClass = MyItem->Actor->GetClass();
		UBlueprint* Blueprint = (ActorClass ? Cast<UBlueprint>(ActorClass->ClassGeneratedBy) : nullptr);

		TWeakObjectPtr<UBlueprint> BlueprintPtr = Blueprint;
		TWeakObjectPtr<UObject>	   ActorPtr = MyItem->Actor;

		if (Blueprint)	// Navigate to blueprint
		{
			auto OnNavigation = [=]
			{
				if (UBlueprint* Bp = BlueprintPtr.Get())
				{
					if (UObject* Asset = ActorPtr.Get())
					{
						Bp->SetObjectBeingDebugged(Asset);
					}
					GEditor->EditObject(Bp);
				}
			};

			return
	            SNew(SHorizontalBox)
	            + SHorizontalBox::Slot()
	            .HAlign(HAlign_Left)
	            .AutoWidth()
	            [
	                SNew(SHyperlink)
	                .Style(FEditorStyle::Get(), "Common.GotoBlueprintHyperlink")
	                .Text(FText::FromName(MyItem->ActorClass))
	                .OnNavigate_Lambda(OnNavigation)
	            ];
		}
		else if (ActorClass && FSourceCodeNavigation::CanNavigateToClass(ActorClass))	// Navigate to cpp code
		{
			auto OnNavigation = [=]
			{
				FSourceCodeNavigation::NavigateToClass(ActorClass);
			};

			return
	            SNew(SHorizontalBox)
	            + SHorizontalBox::Slot()
	            .HAlign(HAlign_Left)
	            .AutoWidth()
	            [
	                SNew(SHyperlink)
	                .Style(FEditorStyle::Get(), "Common.GotoBlueprintHyperlink")
	                .Text(FText::FromName(MyItem->ActorClass))
	                .OnNavigate_Lambda(OnNavigation)
	            ];	
		}
		else	// No navigate
		{
			return
	            SNew(STextBlock)
	            .Text(FText::FromName(MyItem->ActorClass));
		}
	}
	else
	{
		return Super::GenColumnSection(InItem, InColumnName);
	}
}

void USearchConditionTag::Search(TArray<TSharedPtr<void>>& ResultSource)
{
	FString SearchTag = InputBox->GetText().ToString();
	for (FActorIterator It(GWorld.GetReference()); It; ++It)
	{
		for (FName TagName : It->Tags)
		{
			if (TagName.ToString().Find(SearchTag) != INDEX_NONE)
			{
				ResultSource.Emplace(new Item{*It, TagName, It->GetFName(),It->GetClass()->GetFName()});
			}
		}
	}
}

void USearchConditionTag::Select(const TArray<TSharedPtr<void>>& AllSelectedItem)
{
	GEditor->SelectNone(false, true, true);
	for (auto& it : AllSelectedItem)
	{
		Item* MyItem = (Item*)it.Get();
		GEditor->SelectActor(MyItem->Actor, true, true);
	}
}

void USearchConditionTag::OnClickItem(TSharedPtr<void> InItem)
{
	Item* MyItem = (Item*)InItem.Get();
	GEditor->SelectNone(false, true, true);
	GEditor->SelectActor(MyItem->Actor, true, true);
	GUnrealEd->Exec(GWorld.GetReference(), TEXT("CAMERA ALIGN ACTIVEVIEWPORTONLY"));
}
