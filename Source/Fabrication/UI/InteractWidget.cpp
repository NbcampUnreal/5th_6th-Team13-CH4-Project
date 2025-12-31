#include "UI/InteractWidget.h"
#include "Components/Image.h"

void UInteractWidget::SetImage(UTexture2D* InteractImage)
{
	if (InteractType && InteractImage)
	{
		InteractType->SetBrushFromTexture(InteractImage);
	}
}
