// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"
#include "../../Item/ItemLootSubsystem.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	//UE_LOG(LogInventory, Log, TEXT("Inventory Init"));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	const int32 SlotCountPerPage = 24;

	// Page & Equipment slots init
	for (EInventoryCategory Category : TEnumRange<EInventoryCategory>())
	{
		InventoryPages.Add({ Category, FInventoryPage(Category, SlotCountPerPage) });
	}

	for (EEquipmentType Category : TEnumRange<EEquipmentType>())
	{
		EquipmentSlots.Add({Category, FInventorySlot()});
	}

	UE_LOG(LogInventory, Verbose, TEXT("Initialized inventory pages size: %i"), InventoryPages.Num());
}

bool UInventoryComponent::AddItem(FItemInstance& ItemInstance)
{
	if (!ItemInstance.ItemData)
	{
		UE_LOG(LogInventory, Warning, TEXT("AddItem: Item is not valid"));
		return false;
	}

	EItemCategory ItemCategory = ItemInstance.ItemData->Category;
	EInventoryCategory PageCategory = EInventoryCategory::Count;
	switch (ItemCategory)
	{
	case EItemCategory::Equipment:
		PageCategory = EInventoryCategory::Equipment;
		break;
	case EItemCategory::Consumable:
		PageCategory = EInventoryCategory::Consumable;
		break;
	case EItemCategory::Material:
		PageCategory = EInventoryCategory::Material;
		break;
	}
	
	check(PageCategory != EInventoryCategory::Count);

	FInventoryPage& TargetPage = InventoryPages[PageCategory];
	if (TargetPage.AddItem(ItemInstance) && OnInventoryContentChanged.IsBound())
	{
		OnInventoryContentChanged.Execute(PageCategory);
		return true;
	}
	
	return false;
}

void UInventoryComponent::RemoveItem(EInventoryCategory PageCategory, int32 SlotIndex, bool bShouldDropItem)
{
	FInventoryPage& TargetPage = InventoryPages[PageCategory];
	if (TargetPage.IsSlotEmpty(SlotIndex))
	{
		return;
	}

	if (bShouldDropItem)
	{
		const FItemInstance& ItemInstance = TargetPage.GetItemInstance(SlotIndex);

		// spawn actor
		

	}

	TargetPage.RemoveItem(SlotIndex);
}

void UInventoryComponent::SwapItems(EInventoryCategory PageCategory, int32 Index1, int32 Index2)
{
	FInventoryPage& CurrentPage = InventoryPages[PageCategory];
	CurrentPage.SwapItems(Index1, Index2);

	OnInventoryContentChanged.Execute(PageCategory);
}

void UInventoryComponent::TryEquipItem(int32 SlotIndex, EEquipmentType TargetCategory)
{
	FInventoryPage& EquipmentPage = InventoryPages[EInventoryCategory::Equipment];
	const FItemInstance& EquipmentItem = EquipmentPage.GetItemInstance(SlotIndex);
	if (CanEquipItem(EquipmentItem, TargetCategory))
	{
		Swap(EquipmentSlots[TargetCategory], EquipmentPage.Slots[SlotIndex]);
	}

	OnInventoryContentChanged.Execute(EInventoryCategory::Equipment);
}

const FInventoryPage& UInventoryComponent::GetPage(EInventoryCategory PageCategory) const
{
	return InventoryPages[PageCategory];
}

const FInventorySlot& UInventoryComponent::GetEquipmentSlot(EEquipmentType EquipmentType) const
{
	return EquipmentSlots[EquipmentType];
}

bool UInventoryComponent::CanEquipItem(const FItemInstance& Item, EEquipmentType TargetCategory)
{
	if (Item.ItemData == nullptr)
	{
		return true;
	}

	if (const UEquipmentItemData* Equipment = Cast<UEquipmentItemData>(Item.ItemData))
	{
		if (Equipment->EquipmentCategory == TargetCategory)
		{
			return true;
		}
	}

	return false;
}

void UInventoryComponent::EquipItem(const UEquipmentItemData* EquipmentData)
{
	// Apply Item Effect
}

void UInventoryComponent::UnequipItem(EEquipmentType EquipmentType)
{
	// Remove Item Effect
}
