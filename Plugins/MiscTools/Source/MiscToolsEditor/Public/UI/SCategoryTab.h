#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

DECLARE_DELEGATE_OneParam(FOnCategoryChanged, FName);

class MISCTOOLSEDITOR_API SCategoryTab : public SCompoundWidget
{
	public:
	SLATE_BEGIN_ARGS(SCategoryTab)
    : _OnCategoryChanged()
	{}
	SLATE_EVENT(FOnCategoryChanged, OnCategoryChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	void AddCategory(FName Category);

	void SetCurrentCategory(FName Category);

	protected:

	const FSlateBrush* GetTabCheckBoxBorderImage(FName CategoryName) const;
	void OnTabCheckBoxChanged(ECheckBoxState NewState, FName CategoryName);
	ECheckBoxState GetTabCheckBoxCheckedState(FName Category) const;

	private:
	FOnCategoryChanged		OnCategoryChanged;

	FName				CurrentCategory;
	TSharedPtr<class SVerticalBox>	Tab;
	TArray<FName>		AllCategory;
};