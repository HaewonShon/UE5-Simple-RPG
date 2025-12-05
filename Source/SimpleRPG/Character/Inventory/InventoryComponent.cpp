// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	UE_LOG(LogInventory, Log, TEXT("Inventory Init"));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	const int32 SlotCountPerPage = 24;

	for (EInventoryCategory Category : TEnumRange<EInventoryCategory>())
	{
		InventoryPages.Add({ Category, FInventoryPage(Category, SlotCountPerPage) });
	}
	UE_LOG(LogInventory, Log, TEXT("Inventory pages size: %i"), InventoryPages.Num());
}

bool UInventoryComponent::AddItem(FItemInstance ItemInstance)
{
	if (!ItemInstance.ItemData)
	{
		UE_LOG(LogInventory, Log, TEXT("Item is not valid"));
		return false;
	}

	EItemCategory ItemCategory = ItemInstance.ItemData->Category;
	EInventoryCategory InventoryCategory = EInventoryCategory::Count;
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
	
	check(InventoryCategory != EInventoryCategory::Count);

	FInventoryPage& TargetPage = InventoryPages[InventoryCategory];
	UE_LOG(LogInventory, Log, TEXT("Selected page: %i"), TargetPage.Category);
	if (TargetPage.AddItem(ItemInstance) && OnInventoryContentChanged.IsBound())
	{
		OnInventoryContentChanged.Execute(InventoryCategory);
		return true;
	}
	
	return false;
}

const FInventoryPage& UInventoryComponent::GetPage(EInventoryCategory PageCategory) const
{
	return InventoryPages[PageCategory];
}
