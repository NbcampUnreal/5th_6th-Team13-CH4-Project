#include "UI/FCResultWidget.h"

#include "Components/TextBlock.h"

void UFCResultWidget::SetResultText(const FString& Result)
{
	ResultText->SetText(FText::FromString(Result));
}
