// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	for (int8 i = 0; i < static_cast<int8>(EInventoryCategory::INVENTORY_CATEGORY_COUNT); ++i)
	{
		InventoryPages.Add(FInventoryPage(static_cast<EInventoryCategory>(i), InventoryPageSize));
	}
}

bool UInventoryComponent::AddItem(FItemInstance ItemInstance)
{
	if (!ItemInstance.ItemData.IsValid())
	{
		UE_LOG(LogInventory, Log, TEXT("Item is not valid"));
		return false;
	}

	EItemCategory ItemCategory = ItemInstance.ItemData->Category;
	EInventoryCategory InventoryCategory;
	switch (ItemCategory)
	{
	case EItemCategory::Weapon:
	case EItemCategory::Armor:
		InventoryCategory = EInventoryCategory::Equipment;
		break;
	case EItemCategory::Consumable:
		InventoryCategory = EInventoryCategory::Consumable;
		break;
	case EItemCategory::Material:
		InventoryCategory = EInventoryCategory::Material;
		break;
	}
	
	if (!InventoryPages.IsValidIndex(static_cast<int32>(InventoryCategory)))
	{
		UE_LOG(LogInventory, Warning, TEXT("Inventory Page is not valid"));
		return false;
	}

	FInventoryPage& TargetPage = InventoryPages[static_cast<int32>(InventoryCategory)];
	if (TargetPage.AddItem(ItemInstance) && OnInventoryContentChanged.IsBound())
	{
		OnInventoryContentChanged.Execute(InventoryCategory);
		return true;
	}
	
	return false;
}

FInventoryPage& UInventoryComponent::GetPage(EInventoryCategory PageCategory) const
{
	if (!InventoryPages.IsValidIndex(static_cast<int32>(PageCategory)))
	{
		return InventoryPages[static_cast<int32>(PageCategory)];
	}

}
