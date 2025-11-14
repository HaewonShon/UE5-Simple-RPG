// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotGridPanel && SlotWidgetClass.Get())
	{
		SlotGridPanel->SetMinDesiredSlotWidth(128.f);
		SlotGridPanel->SetMinDesiredSlotHeight(128.f);

		for (int32 h = 0; h < PageHeight; ++h)
		{
			for (int32 w = 0; w < PageWidth; ++w)
			{
				SlotGridPanel->AddChildToUniformGrid(CreateWidget<UUserWidget>(this, SlotWidgetClass), h, w);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create Item Slots, %i, %i"), SlotGridPanel == nullptr, SlotWidgetClass == nullptr);
;	}
}

void UInventoryWidget::OnPageSelected(int32 PageIndex)
{
}

void UInventoryWidget::OnCurrentPageSort()
{
}
