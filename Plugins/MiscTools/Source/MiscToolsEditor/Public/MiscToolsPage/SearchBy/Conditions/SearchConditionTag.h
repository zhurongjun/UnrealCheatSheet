#pragma once
#include "MiscToolsPage/SearchBy/SearchByPage.h"
#include "SearchConditionTag.generated.h"

UCLASS()
class MISCTOOLSEDITOR_API USearchConditionTag : public USearchCondition
{
	GENERATED_BODY()
public:
	struct Item
	{
		AActor*		Actor;
		FName		Tag;
		FName		ActorName;
		FName		ActorClass;
	};
	
	virtual FName GetConditionName() override;
	virtual TSharedRef<SWidget> GetSearchInputWidget() override;
	virtual void GetColumnSections(TArray<FName>& OutSections) override;
	virtual TSharedRef<SWidget> GenColumnSection(TSharedPtr<void> InItem, FName InColumnName) override;

	virtual void Search(TArray<TSharedPtr<void>>& ResultSource) override;
	virtual void Select(const TArray<TSharedPtr<void>>& AllSelectedItem) override;
	virtual void OnClickItem(TSharedPtr<void> InItem) override;
private:
	TSharedPtr<SEditableTextBox>		InputBox;
};
