// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

bool FInventorySlot::IsEmpty() const
{
	return Item.ItemData == nullptr;
}

FInventoryPage::FInventoryPage()
{
	//Slots.Add(FInventorySlot());
}

FInventoryPage::FInventoryPage(EInventoryCategory PageCategory, int32 SlotCountPerPage)
{
	Category = PageCategory;

	CountMaxSlot = SlotCountPerPage;
	CountFilledSlot = 0;
	
	for (int32 i = 0; i < CountMaxSlot; ++i)
	{
		Slots.Add(FInventorySlot());
	}
}

bool FInventoryPage::AddItem(FItemInstance ItemInstance)
{
	if (!ItemInstance.ItemData.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemInstance not valid"));
		return false;
	}

	if (!ItemInstance.ItemData->bIsStackable && !HasEmptySlot())
	{
		UE_LOG(LogTemp, Warning, TEXT("no empty slot"));
		return false;
	}

	// If stackable slot exist, place it. Otherwise place in empty slot
	if (ItemInstance.ItemData->bIsStackable)
	{
		FInventorySlot* EmptySlot = nullptr;

		for (FInventorySlot& Slot : Slots)
		{
			if (!Slot.IsEmpty() && ItemInstance.ItemID == Slot.Item.ItemID)
			{
				Slot.Item.AddStack(ItemInstance);
				if (ItemInstance.StackCount == 0)
				{
					return true;
				}
			}
			else if(EmptySlot == nullptr)
			{
				EmptySlot = &Slot;
			}
		}

		if (EmptySlot)
		{
			EmptySlot->Item = ItemInstance;
			++CountFilledSlot;
			return true;
		}
	}
	else
	{
		// Find 1st empty slot and place item
		for (FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.Item = ItemInstance;
				++CountFilledSlot;
				return true;
			}
		}
	}

	return false;
}

bool FInventoryPage::HasEmptySlot() const
{
	return CountFilledSlot < CountMaxSlot;
}
