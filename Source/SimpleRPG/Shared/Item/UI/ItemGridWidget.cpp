// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Item/UI/ItemGridWidget.h"
#include "Components/UniformGridPanel.h"

void UItemGridWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Slot Setup
	if (SlotGridPanel && SlotWidgetClass.Get())
	{
		for (int32 h = 0; h < PageHeight; ++h)
		{
			for (int32 w = 0; w < PageWidth; ++w)
			{
				UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
				SlotGridPanel->AddChildToUniformGrid(SlotWidget, h, w);
				if (SlotWidget)
				{
					SlotWidget->OnDragBegin.BindUObject(this, &UItemGridWidget::OnSlotDragBegin);
					SlotWidget->OnDrop.BindUObject(this, &UItemGridWidget::OnSlotsSwapped);
					SlotWidget->OnDoubleClick.BindUObject(this, &UItemGridWidget::OnSlotDoubleClicked);
					SlotWidget->OnHovered.BindUObject(this, &UItemGridWidget::OnSlotHovered);
					SlotWidget->OnHoverEnded.BindUObject(this, &UItemGridWidget::OnSlotHoverEnded);
				}
			}
		}
	}
}

UItemSlotWidget* UItemGridWidget::GetSlotAt(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= PageWidth * PageHeight)
	{
		return nullptr;
	}

	return Cast<UItemSlotWidget>(SlotGridPanel->GetChildAt(SlotIndex));
}

TArray<UWidget*> UItemGridWidget::GetAllSlots()
{
	return SlotGridPanel->GetAllChildren();
}

void UItemGridWidget::SetSlotType(ESlotType SlotType)
{
	for (int32 h = 0; h < PageHeight; ++h)
	{
		for (int32 w = 0; w < PageWidth; ++w)
		{
			UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(SlotGridPanel->GetChildAt(h * PageWidth + w));
			if (SlotWidget)
			{
				SlotWidget->SetSlotAddress(FSlotAddress{ SlotType, h * PageWidth + w });
			}
		}
	}
}

void UItemGridWidget::OnSlotDragBegin(const FSlotAddress& SlotAddress)
{
	OnDragBegin.Broadcast(SlotAddress);
}

void UItemGridWidget::OnSlotsSwapped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	OnDrop.Broadcast(SourceSlotAddress, TargetSlotAddress);
}

void UItemGridWidget::OnSlotDoubleClicked(const FSlotAddress& SlotAddress)
{
	OnDoubleClick.Broadcast(SlotAddress);
}

void UItemGridWidget::OnSlotHovered(const FSlotAddress& SlotAddress)
{
	OnHovered.Broadcast(SlotAddress);
}

void UItemGridWidget::OnSlotHoverEnded()
{
	OnHoverEnded.Broadcast();
}
