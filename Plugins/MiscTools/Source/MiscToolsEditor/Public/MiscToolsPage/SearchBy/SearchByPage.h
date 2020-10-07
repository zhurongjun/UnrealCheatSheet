#pragma once
#include "CoreMinimal.h"
#include "MiscToolsPage/MiscToolsPage.h"
#include "Widgets/Views/SListView.h"
#include "SearchByPage.generated.h"

class USearchCondition;
class SWidget;

UCLASS()
class MISCTOOLSEDITOR_API USearchByPage : public UMiscToolsPage
{
	GENERATED_BODY()
	using SSearchResultList = SListView<TSharedPtr<void>>;

	void _RebuildPage();
public:
	// ~Begin UMiscToolsPage API
	virtual TSharedRef<SWidget> GetPageContent() override;
	virtual FName GetPageName() override;
	// ~End UMiscToolsPage API
private:
	void CollectSearchConditions();

	// head
	void OnSearchConditionSelectionChanged(TSharedPtr<FName> InName, ESelectInfo::Type InSelectInfo);
	FReply OnSearch();
	
	// build widgets 
	TSharedRef<SWidget> BuildHead();
	TSharedRef<SWidget> BuildResult();
	
	// SSearchResultList call back
 	TSharedRef<ITableRow> OnSearchResultListGenRow(TSharedPtr<void> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnResultSelectionChanged(TSharedPtr<void> InItem, ESelectInfo::Type SelectInfo);
	void OnDoubleClickResult(TSharedPtr<void> ResultItem);

private:
	UPROPERTY()
	TArray<USearchCondition*>	SearchConditions;
	USearchCondition*			CurrentCondition;

	// config
	FName		CurrentSearchConditionName;
	FName		CurrentSearchCategory;
	
	// head
	TArray<TSharedPtr<FName>>		SearchConditionNames;
	
	// search table
	TSharedPtr<SBox>		SearchTable;

	// search result 
	TSharedPtr<SSearchResultList> 	SearchResult;
	TArray<TSharedPtr<void>>		ResultSource;
};

UCLASS(Abstract)
class MISCTOOLSEDITOR_API USearchCondition : public UObject
{
	GENERATED_BODY()
public:
	virtual FName GetConditionName();

	// build widget 
	virtual TSharedRef<SWidget> GetSearchInputWidget();
	virtual void GetColumnSections(TArray<FName>& OutSections);
	virtual TSharedRef<SWidget> GenColumnSection(TSharedPtr<void> InItem,FName InColumnName);

	// item functions  
	virtual void Search(TArray<TSharedPtr<void>>& ResultSource);
	virtual void Select(const TArray<TSharedPtr<void>>& AllSelectedItem);
	virtual void OnClickItem(TSharedPtr<void> InItem);
};
