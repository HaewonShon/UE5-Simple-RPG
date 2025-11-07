// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	for (int8 i = 0; i < static_cast<int8>(EInventoryCategory::INVENTORY_CATEGORY_COUNT); ++i)
	{
		InventoryPages.Add(FInventoryPage(static_cast<EInventoryCategory>(i)));
	}
}

bool UInventoryComponent::AddItem(UItemData* Item)
{
	if (!Item)
	{
		UE_LOG(LogInventory, Log, TEXT("Item is not valid"));
	}

	EItemCategory ItemCategory = Item->Category;
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


	
	return false;
}
