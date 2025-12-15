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

	for (EInventoryCategory Category : TEnumRange<EInventoryCategory>())
	{
		InventoryPages.Add({ Category, FInventoryPage(Category, SlotCountPerPage) });
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
	case EItemCategory::Weapon:
	case EItemCategory::Armor:
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
	UE_LOG(LogInventory, Verbose, TEXT("Selected page: %i"), TargetPage.Category);
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

const FInventoryPage& UInventoryComponent::GetPage(EInventoryCategory PageCategory) const
{
	return InventoryPages[PageCategory];
}
