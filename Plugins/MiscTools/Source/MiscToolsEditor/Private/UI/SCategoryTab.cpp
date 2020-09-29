#include "UI/SCategoryTab.h"
#include "SlateOptMacros.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSpacer.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCategoryTab::Construct(const FArguments& InArgs)
{
	OnCategoryChanged = InArgs._OnCategoryChanged;

	auto Tabs = SNew(SVerticalBox);
	Tab = Tabs;

	ChildSlot
	[
		Tabs
	];

}
void SCategoryTab::AddCategory(FName Category)
{
	if (AllCategory.Contains(Category)) return;
	AllCategory.Push(Category);

	Tab->AddSlot()
	.AutoHeight()
	[
		SNew(SCheckBox)
		.Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
		.OnCheckStateChanged_Raw(this, &SCategoryTab::OnTabCheckBoxChanged, Category)
		.IsChecked_Raw(this, &SCategoryTab::GetTabCheckBoxCheckedState, Category)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(SSpacer)
				.Size(FVector2D(1, 30))
			]
			+ SOverlay::Slot()
			.Padding(FMargin(6, 0, 15, 0))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "PlacementBrowser.Tab.Text")
				.Text(FText::FromName(Category))
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			[
				SNew(SImage)
				.Image_Raw(this, &SCategoryTab::GetTabCheckBoxBorderImage, Category)
			]
		]
	];
}
void SCategoryTab::SetCurrentCategory(FName Category)
{
	if (!AllCategory.Contains(Category)) return;
	if (CurrentCategory != Category)
	{
		CurrentCategory = Category;
		OnCategoryChanged.Execute(Category);
	}
}
const FSlateBrush * SCategoryTab::GetTabCheckBoxBorderImage(FName CategoryName) const
{
	if (CurrentCategory == CategoryName)
	{
		static FName ActiveTabButtonBorder("PlacementBrowser.ActiveTabBar");
		return FEditorStyle::GetBrush(ActiveTabButtonBorder);
	}
	return nullptr;
}
void SCategoryTab::OnTabCheckBoxChanged(ECheckBoxState NewState, FName CategoryName)
{
	if (NewState == ECheckBoxState::Checked && CurrentCategory != CategoryName)
	{
		CurrentCategory = CategoryName;
		if (OnCategoryChanged.IsBound())
			OnCategoryChanged.Execute(CategoryName);
	}
}
ECheckBoxState SCategoryTab::GetTabCheckBoxCheckedState(FName Category) const
{
	return CurrentCategory == Category ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION