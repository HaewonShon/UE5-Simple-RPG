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

const FItemInstance& FInventoryPage::GetItemInstance(int32 SlotIndex)
{
	return Slots[SlotIndex].Item;
}

bool FInventoryPage::IsSlotEmpty(int32 SlotIndex) const
{
	return Slots[SlotIndex].IsEmpty();
}

bool FInventoryPage::AddItem(FItemInstance& ItemInstance)
{
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
				else
				{
					return false;
				}
			}
			else if(EmptySlot == nullptr && Slot.IsEmpty())
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

void FInventoryPage::RemoveItem(int32 SlotIndex)
{
	
}

void FInventoryPage::SwapItems(int32 Index1, int32 Index2)
{
	if (Index1 < 0 || Index1 >= CountMaxSlot || Index2 < 0 || Index2 >= CountMaxSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory Index not valid"));
		return;
	}

	Swap(Slots[Index1], Slots[Index2]);
}

bool FInventoryPage::HasEmptySlot() const
{
	return CountFilledSlot < CountMaxSlot;
}
