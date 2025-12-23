// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"
#include "Components/InvalidationBox.h"
#include "InventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "../../Character/SimpleRPGPlayerState.h"
#include "../../Character/Inventory/InventoryComponent.h"
#include "InventorySlotDragWidget.h"

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
				UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
				SlotGridPanel->AddChildToUniformGrid(SlotWidget, h, w);
				if (SlotWidget)
				{
					SlotWidget->OnDragBegin.BindUObject(this, &UInventoryWidget::OnSlotDragBegin);
					SlotWidget->SetIndex(h * PageWidth + w);
					SlotWidget->SlotType = ESlotType::Storage;
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

	// Equipment Slot Setup
	EquipmentSlotMap.Add({ ESlotType::Helmet, HelmetSlot.Get() });
	EquipmentSlotMap.Add({ ESlotType::Chest, ChestSlot.Get() });
	EquipmentSlotMap.Add({ ESlotType::Pants, PantsSlot.Get() });
	EquipmentSlotMap.Add({ ESlotType::Boots, BootsSlot.Get() });
	EquipmentSlotMap.Add({ ESlotType::Weapon, WeaponSlot.Get() });

	// Register Inventory component
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState <ASimpleRPGPlayerState>())
	{
		InventoryComponent = PlayerState->GetInventoryComponent();
		check(InventoryComponent != nullptr);

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

	//UUserWidget* Widget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
	SlotVisualWidget = CreateWidget<UInventorySlotDragWidget>(GetOwningPlayer(), SlotVisualWidgetClass);
	if (SlotVisualWidget)
	{
		SlotVisualWidget->SetDesiredSize(FVector2D{ SlotGridPanel->GetMinDesiredSlotWidth(), SlotGridPanel->GetMinDesiredSlotHeight() });
		SlotVisualWidget->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("Set Desired Size: %f"), SlotGridPanel->GetMinDesiredSlotWidth());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create SlotVisualWidget"));
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

void UInventoryWidget::OnSlotDragBegin(FSlotInfo SlotInfo)
{
	UE_LOG(LogInventory, Log, TEXT("Slot Drag detected"));

	UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(SlotInfo.SlotIndex));
	if (!SlotWidget)
	{
		UE_LOG(LogInventory, Log, TEXT("Failed to cast InventorySlotWidget"));
		return;
	}

	UDragDropOperation*& DragOperation = SlotWidget->DragDropOperationRef;
	DragOperation->DefaultDragVisual = SlotVisualWidget;
	SlotVisualWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlotVisualWidget->OnDragBegin(SlotWidget->GetIconTexture());
}

void UInventoryWidget::OnSwapSlots(FSlotInfo Slot1, FSlotInfo Slot2)
{
	// no support for swap between different equipment slots
	if (Slot1.SlotType != ESlotType::Storage && Slot2.SlotType != ESlotType::Storage)
	{
		return;
	}

	if (Slot1.SlotType == ESlotType::Storage && Slot2.SlotType == ESlotType::Storage)
	{
		InventoryComponent->SwapItems(SelectedPage, Slot1.SlotIndex, Slot2.SlotIndex);
	}
	else if (Slot1.SlotType == ESlotType::Storage) // Storage->Equipment
	{
		
	}
	else if (Slot1.SlotType != ESlotType::Storage) // Equipment->Storage
	{

	}
	
	// find target slot
	

	// is empty -> ±×³É ÀåÂø
	// else swap

	UpdateCurrentPageContents();
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