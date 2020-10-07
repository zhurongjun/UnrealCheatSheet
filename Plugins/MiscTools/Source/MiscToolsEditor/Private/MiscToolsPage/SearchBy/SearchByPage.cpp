#include "MiscToolsPage/SearchBy/SearchByPage.h"

#include "SNameComboBox.h"

void USearchByPage::_RebuildPage()
{	
	// get current search condition 
	CurrentCondition = nullptr;
	for (USearchCondition* Condition : SearchConditions)
	{
		if (Condition->GetConditionName() == CurrentSearchConditionName)
		{
			CurrentCondition = Condition;
			break;
		}
	}
	
	// rebuild search table
	if (CurrentCondition != nullptr)
	{
		SearchTable->SetContent(CurrentCondition->GetSearchInputWidget());
	}
	
	// rebuild head row 
	if (CurrentCondition != nullptr)
	{
		TArray<FName> Columns;
		CurrentCondition->GetColumnSections(Columns);
		TSharedPtr<SHeaderRow> HeadRow = SearchResult->GetHeaderRow();
		HeadRow->ClearColumns();
		for (FName ColumnName : Columns)
		{
			HeadRow->AddColumn(SHeaderRow::Column(ColumnName).DefaultLabel(FText::FromName(ColumnName)));
		}
	}
	
	// clear result list
	ResultSource.Reset();
	SearchResult->RequestListRefresh();
}

TSharedRef<SWidget> USearchByPage::GetPageContent()
{
	CollectSearchConditions();
	
	TSharedRef<SWidget> Ret = SNew(SVerticalBox)
	+SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.AutoHeight()
	[
		BuildHead()
	]
	+ SVerticalBox::Slot()
	.VAlign(VAlign_Fill)
	[
		BuildResult()
	];

	_RebuildPage();
	return Ret;
}

FName USearchByPage::GetPageName()
{
	static FName PageName(TEXT("SearchBy"));
	return PageName;
}

void USearchByPage::CollectSearchConditions()
{
	SearchConditions.Reset();
	SearchConditionNames.Reset();
	TArray<UClass*>	AllSearchConditionClasses;
	GetDerivedClasses(USearchCondition::StaticClass(), AllSearchConditionClasses, true);
	for (UClass* SearchConditionClass : AllSearchConditionClasses)
	{
		// abstract 
		if (SearchConditionClass->ClassFlags & EClassFlags::CLASS_Abstract) continue;

		// get defaultObject 
		USearchCondition* SearchCondition = Cast<USearchCondition>(SearchConditionClass->GetDefaultObject(true));

		// add to list 
		if (SearchCondition)
		{
			this->SearchConditions.Add(SearchCondition);
			SearchConditionNames.Add(MakeShareable(new FName(SearchCondition->GetConditionName())));
		}
	}

	if (SearchConditionNames.Num() == 0)
	{
		SearchConditionNames.Add(MakeShareable(new FName(TEXT("NONE"))));
	}

	CurrentSearchConditionName = *SearchConditionNames[0];
}

void USearchByPage::OnSearchConditionSelectionChanged(TSharedPtr<FName> InName, ESelectInfo::Type InSelectInfo)
{
	CurrentSearchConditionName = *InName;
	_RebuildPage();
}

FReply USearchByPage::OnSearch()
{
	if (CurrentCondition)
	{
		ResultSource.Reset();
		CurrentCondition->Search(ResultSource);
		SearchResult->RequestListRefresh();
	}
	return FReply::Handled();
}

TSharedRef<SWidget> USearchByPage::BuildHead()
{
	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.VAlign(VAlign_Top)
	.AutoHeight()
	[
		// config tab 
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),12))
			.Text(FText::FromString(TEXT("Search Actors By : ")))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SNameComboBox)
			.OptionsSource(&SearchConditionNames)
			.InitiallySelectedItem(SearchConditionNames[0])
			.OnSelectionChanged_UObject(this, &USearchByPage::OnSearchConditionSelectionChanged)
		]
	]
	+ SVerticalBox::Slot()
    .VAlign(VAlign_Top)
    .AutoHeight()
    [
    	// search tab 
    	SNew(SHorizontalBox)
    	+ SHorizontalBox::Slot()
    	.HAlign(HAlign_Fill)
    	.VAlign(VAlign_Center)
    	[
    		SAssignNew(SearchTable ,SBox)
    		.HAlign(HAlign_Fill)
    		.VAlign(VAlign_Fill)
    	]
    	+ SHorizontalBox::Slot()
    	.HAlign(HAlign_Right)
    	.VAlign(VAlign_Center)
    	.AutoWidth()
    	.Padding(10,0,0,0)
    	[
    		SNew(SButton)
    		.OnClicked_UObject(this, &USearchByPage::OnSearch)
    		[
    			SNew(STextBlock)
    			.MinDesiredWidth(50)
    			.Justification(ETextJustify::Center)
    			.Text(FText::FromString(TEXT("Search")))
    		]
    	]
    ];
}

TSharedRef<SWidget> USearchByPage::BuildResult()
{
	return SAssignNew(SearchResult, SSearchResultList)
	.ItemHeight(24)
    .ListItemsSource(&ResultSource)
    .OnGenerateRow_UObject(this, &USearchByPage::OnSearchResultListGenRow)
    .OnSelectionChanged_UObject(this,&USearchByPage::OnResultSelectionChanged)
    .OnMouseButtonDoubleClick_UObject(this,&USearchByPage::OnDoubleClickResult)
    .HeaderRow
    (
        SNew(SHeaderRow)
    );
}

TSharedRef<ITableRow> USearchByPage::OnSearchResultListGenRow(TSharedPtr<void> InItem,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	class SActorItemWidget : public SMultiColumnTableRow<TSharedPtr<void>>
	{
	public:
		SLATE_BEGIN_ARGS(SActorItemWidget) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, TSharedPtr<void> InListItem, USearchCondition* InCondition)
		{
			Item = InListItem;
			Condition = InCondition;
			SMultiColumnTableRow<TSharedPtr<void>>::Construct(FSuperRowType::FArguments(), InOwnerTable);
		}

		 TSharedRef<SWidget>GenerateWidgetForColumn(const FName& ColumnName)
		{
			if (Condition)
				return Condition->GenColumnSection(Item, ColumnName);
			else
				return SNew(STextBlock)
				.Text(FText::FromString(TEXT("No Condition!!")))
				.ColorAndOpacity(FLinearColor::Red);
		}
		USearchCondition*	Condition;
		TSharedPtr<void> Item;
	};

	return SNew(SActorItemWidget, OwnerTable, InItem, CurrentCondition);
}

void USearchByPage::OnResultSelectionChanged(TSharedPtr<void> InItem, ESelectInfo::Type SelectInfo)
{
	if (CurrentCondition)
	{
		TArray<TSharedPtr<void>> SelectedItems;
		SearchResult->GetSelectedItems(SelectedItems);
		CurrentCondition->Select(SelectedItems);
	}
}

void USearchByPage::OnDoubleClickResult(TSharedPtr<void> ResultItem)
{
	if (CurrentCondition)
	{
		CurrentCondition->OnClickItem(ResultItem);
	}
}

//=================================================Search condition=================================================
FName USearchCondition::GetConditionName()
{
	return NAME_None;
}

TSharedRef<SWidget> USearchCondition::GetSearchInputWidget()
{
	return SNew(STextBlock)
	.Text(FText::FromString(TEXT("Overried GetSearchInputWidget() to specify search widget")));
}

void USearchCondition::Search(TArray<TSharedPtr<void>>& ResultSource)
{
}

void USearchCondition::Select(const TArray<TSharedPtr<void>>& AllSelectedItem)
{
}

void USearchCondition::OnClickItem(TSharedPtr<void> InItem)
{
}

void USearchCondition::GetColumnSections(TArray<FName>& OutSections)
{
}

TSharedRef<SWidget> USearchCondition::GenColumnSection(TSharedPtr<void> InItem, FName InColumnName)
{
	return SNew(STextBlock)
	.Text(FText::FromString(TEXT("Overried GenColumnSection() to specify column section")));
}

