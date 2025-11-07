// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

FInventoryPage::FInventoryPage()
{
	Slots.Add(FInventorySlot());
}

FInventoryPage::FInventoryPage(EInventoryCategory PageCategory)
{
	Category = PageCategory;
}

bool FInventoryPage::AddItem(FItemInstance Item)
{
	// Find 1st empty slot
	for (int32 index = 0; index < Slots.Num(); ++index)
	{
		FInventorySlot& Slot = Slots[index];
		if (Slot.Item.ItemData == nullptr)
		{
			Slot.Item = Item;
			return true;
		}
	}

	return false;
}
