// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlotWidget.h"
#include "../Character/SimpleRPGPlayerState.h"
#include "../Character/Inventory/InventoryComponent.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{

}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Inventory Slot Setup
	if (SlotGridPanel && SlotWidgetClass.Get())
	{
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
		UE_LOG(LogInventory, Warning, TEXT("Failed to create Item Slots, %i, %i"), SlotGridPanel == nullptr, SlotWidgetClass == nullptr);
	}

	// Register Inventory component
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState < ASimpleRPGPlayerState>())
	{
		InventoryComponent = PlayerState->GetInventoryComponent();

		if (InventoryComponent.IsValid())
		{
			InventoryComponent->OnInventoryContentChanged.BindUObject(this, &UInventoryWidget::OnContentChanged);
			UE_LOG(LogInventory, Log, TEXT("Inventory Component bound to ui"));
		}
	}

	SelectedPage = EInventoryCategory::Equipment;
}

void UInventoryWidget::OnPageSelected(int32 PageIndex)
{
	SelectedPage = static_cast<EInventoryCategory>(PageIndex);

}

void UInventoryWidget::OnCurrentPageSort()
{

}

void UInventoryWidget::OnContentChanged(EInventoryCategory ChangedPageCategory)
{
	UE_LOG(LogInventory, Log, TEXT("Inventory Widget OnChanged Called"));
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		return;
	}

	if (ChangedPageCategory == SelectedPage)
	{
		UpdateCurrentPageContents();
	}
}

void UInventoryWidget::UpdateCurrentPageContents()
{
	if (!InventoryComponent.IsValid())
	{
		return;
	}

	FInventoryPage& Page = InventoryComponent->GetPage(SelectedPage);
	for (int32 Index = 0; Index < PageWidth * PageHeight; ++Index)
	{
		if (!Page.Slots[Index].IsEmpty())
		{
			UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(Index));
			SlotWidget->SetItem(&Page.Slots[Index].Item);
		}
	}
}