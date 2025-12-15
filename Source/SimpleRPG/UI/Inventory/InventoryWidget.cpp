// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"
#include "Components/InvalidationBox.h"
#include "InventorySlotWidget.h"
#include "../../Character/SimpleRPGPlayerState.h"
#include "../../Character/Inventory/InventoryComponent.h"

#include "Misc/OutputDeviceDebug.h"

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
				UUserWidget* Widget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
				SlotGridPanel->AddChildToUniformGrid(Widget, h, w);
				if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(Widget))
				{
					SlotWidget->OnDragBegin.BindUObject(this, &UInventoryWidget::OnSlotDragBegin);
				}
				else
				{
					UE_LOG(LogInventory, Warning, TEXT("Failed to bind drag function."));
				}
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
		OnNativeVisibilityChanged.AddUObject(this, &UInventoryWidget::OnInventoryToggled);
	}

	SelectedPage = EInventoryCategory::Equipment;
	for (EInventoryCategory InventoryCategory : TEnumRange<EInventoryCategory>())
	{
		bIsPageContentChanged.Add({InventoryCategory, true});
	}
}

void UInventoryWidget::OnPageSelected(int32 PageIndex)
{
	SelectedPage = static_cast<EInventoryCategory>(PageIndex);
	UpdateCurrentPageContents();
}

void UInventoryWidget::OnCurrentPageSort()
{
	
}

void UInventoryWidget::OnInventoryToggled(ESlateVisibility ChangedVisibility)
{
	if (bIsPageContentChanged[SelectedPage])
	{
		UpdateCurrentPageContents();
	}
}

void UInventoryWidget::OnSlotDragBegin(UInventorySlotWidget* SlotWidget)
{
	UE_LOG(LogInventory, Log, TEXT("Slot Drag detected"));
}

void UInventoryWidget::OnContentChanged(EInventoryCategory ChangedPageCategory)
{
	UE_LOG(LogInventory, Log, TEXT("Inventory Widget OnChanged Called"));

	bIsPageContentChanged[ChangedPageCategory] = true;
	if (GetVisibility() != ESlateVisibility::Collapsed && ChangedPageCategory == SelectedPage)
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

	const FInventoryPage& Page = InventoryComponent->GetPage(SelectedPage);
	for (int32 Index = 0; Index < PageWidth * PageHeight; ++Index)
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(Index));
		if (!Page.Slots[Index].IsEmpty())
		{
			SlotWidget->SetItem(&Page.Slots[Index].Item);
		}
		else
		{
			SlotWidget->ClearItem();
		}
	}
	UE_LOG(LogInventory, Log, TEXT("Inventory Widget Updated page %i"), SelectedPage);
	bIsPageContentChanged[SelectedPage] = false;

}