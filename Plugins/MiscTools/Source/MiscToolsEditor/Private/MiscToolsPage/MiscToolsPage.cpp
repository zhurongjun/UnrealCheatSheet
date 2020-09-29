#include "MiscToolsPage/MiscToolsPage.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<SWidget> UMiscToolsPage::GetPageContent()
{
	return SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .HAlign(HAlign_Center)
    .VAlign(VAlign_Center)
    [
        SNew(STextBlock)
        .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"),30))
        .ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
        .Text(FText::FromString(TEXT("Fill your content by override GetPageContent()")))
    ];
}

FName UMiscToolsPage::GetPageName()
{
	return NAME_None;
}
