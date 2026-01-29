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
#include "BackdropWidget.h"
#include "ItemDescriptionWidget.h"
#include "Misc/OutputDeviceDebug.h"
#include "Blueprint/WidgetLayoutLibrary.h"

EEquipmentType ConvertSlotTypeToEquipmentType(ESlotType SlotType)
{
	if (SlotType == ESlotType::Helmet) return EEquipmentType::Helmet;
	if (SlotType == ESlotType::Chest) return EEquipmentType::Chest;
	if (SlotType == ESlotType::Pants) return EEquipmentType::Pants;
	if (SlotType == ESlotType::Boots) return EEquipmentType::Boots;
	if (SlotType == ESlotType::Weapon) return EEquipmentType::Weapon;
	return EEquipmentType::Count;
}

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
					SlotWidget->OnDrop.BindUObject(this, &UInventoryWidget::OnSlotsSwapped);
					SlotWidget->OnDoubleClick.BindUObject(this, &UInventoryWidget::OnItemUsed);
					SlotWidget->OnHovered.BindUObject(this, &UInventoryWidget::OnSlotHovered);
					SlotWidget->OnHoverEnded.BindUObject(this, &UInventoryWidget::OnSlotHoverEnded);
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
	{
		EquipmentSlotMap.Add({ ESlotType::Helmet, HelmetSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Chest, ChestSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Pants, PantsSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Boots, BootsSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Weapon, WeaponSlot.Get() });

		for(ESlotType SlotType : TEnumRange<ESlotType>())
		{
			if (SlotType == ESlotType::Storage) continue;

			UInventorySlotWidget* SlotWidget = EquipmentSlotMap[SlotType];
			SlotWidget->SlotType = SlotType;
			SlotWidget->OnDragBegin.BindUObject(this, &UInventoryWidget::OnSlotDragBegin);
			SlotWidget->OnDrop.BindUObject(this, &UInventoryWidget::OnSlotsSwapped);
			SlotWidget->OnDoubleClick.BindUObject(this, &UInventoryWidget::OnItemUsed);
			SlotWidget->OnHovered.BindUObject(this, &UInventoryWidget::OnSlotHovered);
			SlotWidget->OnHoverEnded.BindUObject(this, &UInventoryWidget::OnSlotHoverEnded);
		}
	}

	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		// Register components from PS
		InventoryComponentRef = PlayerState->GetInventoryComponent();
		check(InventoryComponentRef.IsValid());

		InventoryComponentRef->OnInventoryContentChanged.BindUObject(this, &UInventoryWidget::OnContentChanged);
		InventoryComponentRef->OnEquipmentContentChanged.BindUObject(this, &UInventoryWidget::OnEquipmentChanged);
		UE_LOG(LogInventory, Log, TEXT("Inventory Component bound to ui"));
		
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

void UInventoryWidget::BindItemDiscardDelegate(UBackdropWidget* Widget)
{
	if (!Widget)
	{
		UE_LOG(LogInventory, Warning, TEXT("Backdrop widget is not valid"));
		return;
	}

	Widget->OnItemDiscard.BindUObject(this, &UInventoryWidget::OnItemDiscarded);
	UE_LOG(LogInventory, Log, TEXT("OnItemDiscard Registtered"));
}

void UInventoryWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidgetRef)
{
	ItemDescriptionWidgetRef = DescriptionWidgetRef;
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

	UInventorySlotWidget* SlotWidget;
	if (SlotInfo.SlotType == ESlotType::Storage)
	{
		SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(SlotInfo.SlotIndex));
	}
	else
	{
		SlotWidget = EquipmentSlotMap[SlotInfo.SlotType];
	}

	if (!SlotWidget)
	{
		UE_LOG(LogInventory, Log, TEXT("Failed to cast InventorySlotWidget"));
		return;
	}

	UDragDropOperation*& DragOperation = SlotWidget->DragDropOperationRef;
	if (DragOperation)
	{
		DragOperation->DefaultDragVisual = SlotVisualWidget;
		SlotVisualWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SlotVisualWidget->OnDragBegin(SlotWidget->GetIconTexture());
	}
	else
	{
		UE_LOG(LogInventory, Warning, TEXT("DragOperation not valid"));
	}
}

void UInventoryWidget::OnSlotsSwapped(FSlotInfo Slot1, FSlotInfo Slot2)
{
	// no support for swap between different equipment slots
	if (Slot1.SlotType != ESlotType::Storage && Slot2.SlotType != ESlotType::Storage)
	{
		return;
	}

	if (Slot1.SlotType == ESlotType::Storage && Slot2.SlotType == ESlotType::Storage)
	{
		InventoryComponentRef->SwapItems(SelectedPage, Slot1.SlotIndex, Slot2.SlotIndex);
	}
	else if (SelectedPage == EInventoryCategory::Equipment && Slot1.SlotType == ESlotType::Storage) // Storage->Equipment
	{
		InventoryComponentRef->TryEquipItem(Slot1.SlotIndex, ConvertSlotTypeToEquipmentType(Slot2.SlotType));
		OnEquipmentChanged();
	}
	else if (SelectedPage == EInventoryCategory::Equipment && Slot1.SlotType != ESlotType::Storage) // Equipment->Storage
	{
		InventoryComponentRef->TryEquipItem(Slot2.SlotIndex, ConvertSlotTypeToEquipmentType(Slot1.SlotType));
		OnEquipmentChanged();
	}

	UpdateCurrentPageContents();
}

void UInventoryWidget::OnItemDiscarded(FSlotInfo SlotWidget)
{
	UE_LOG(LogInventory, Verbose, TEXT("Widget OnItemDiscard %i"), SlotWidget.SlotIndex);
	InventoryComponentRef->RemoveItem(SelectedPage, SlotWidget.SlotIndex, true);
}

void UInventoryWidget::OnItemUsed(FSlotInfo SlotWidget)
{
	UE_LOG(LogInventory, Verbose, TEXT("Widget OnItemUsed %i"), SlotWidget.SlotIndex);

	if (SlotWidget.SlotType != ESlotType::Storage)
	{
		InventoryComponentRef->TryRemoveEquipment(ConvertSlotTypeToEquipmentType(SlotWidget.SlotType));
	}
	else if (SelectedPage == EInventoryCategory::Equipment)
	{
		InventoryComponentRef->TryEquipItem(SlotWidget.SlotIndex);
	}
	else
	{
		InventoryComponentRef->UseItem(SelectedPage, SlotWidget.SlotIndex);
	}
}

void UInventoryWidget::OnSlotHovered(FSlotInfo SlotWidget)
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnSlotHovered Called"));
	FItemDescription Description;
	if (SlotWidget.SlotType != ESlotType::Storage)
	{
		Description = InventoryComponentRef->GetItemDescription(ConvertSlotTypeToEquipmentType(SlotWidget.SlotType));
	}
	else
	{
		Description = InventoryComponentRef->GetItemDescription(SelectedPage, SlotWidget.SlotIndex);
	}

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInViewport(MousePos, false);
}

void UInventoryWidget::OnSlotHoverEnded()
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnSlotHoverEnded Called"));
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::OnContentChanged(EInventoryCategory ChangedPageCategory)
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnChanged Called"));
	bIsPageContentChanged[ChangedPageCategory] = true;
	if (GetVisibility() != ESlateVisibility::Collapsed && ChangedPageCategory == SelectedPage)
	{
		UpdateCurrentPageContents();
	}
}

void UInventoryWidget::OnEquipmentChanged()
{
	EEquipmentType Types[5] = 
		{ EEquipmentType::Helmet, EEquipmentType::Chest, EEquipmentType::Pants, EEquipmentType::Boots, EEquipmentType::Weapon };
	ESlotType SlotTypes[5] =
		{ ESlotType::Helmet, ESlotType::Chest, ESlotType::Pants, ESlotType::Boots, ESlotType::Weapon };

	for (int32 i = 0; i < 5; ++i)
	{
		const FInventorySlot& ItemSlot = InventoryComponentRef->GetEquipmentSlot(Types[i]);
		UInventorySlotWidget* SlotWidget = EquipmentSlotMap[SlotTypes[i]];

		if (!ItemSlot.IsEmpty())
		{
			SlotWidget->SetItem(ItemSlot.Item);
		}
		else
		{
			SlotWidget->ClearItem();
		}
	}
}

void UInventoryWidget::UpdateCurrentPageContents()
{
	const FInventoryPage& Page = InventoryComponentRef->GetPage(SelectedPage);
	for (int32 Index = 0; Index < PageWidth * PageHeight; ++Index)
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(Index));
		if (!Page.Slots[Index].IsEmpty())
		{
			SlotWidget->SetItem(Page.Slots[Index].Item);
		}
		else
		{
			SlotWidget->ClearItem();
		}
	}
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget Updated page %i"), SelectedPage);
	bIsPageContentChanged[SelectedPage] = false;
}