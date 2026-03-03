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

FInventoryPage::FInventoryPage(int32 SlotCountPerPage)
{
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
		UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item1"));

	if (ItemInstance.ItemData->bIsStackable)
	{
		// If stackable slot exist, place it first.
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
		}
		UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item2"));

		// place left items in empty slotss
		for (FInventorySlot& Slot : Slots)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item3"));
			if (Slot.IsEmpty())
			{
				Slot.Item.SetItem(ItemInstance.ItemData, FMath::Min(ItemInstance.ItemData->MaxStackSize, ItemInstance.StackCount));
				++CountFilledSlot;

				ItemInstance.StackCount -= Slot.Item.StackCount;
				if (ItemInstance.StackCount == 0)
				{
					return true;
				}
			}
		}
	}
	else
	{
		// place left items in empty slots
		for (FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.Item.SetItem(ItemInstance.ItemData,
					FMath::Min(ItemInstance.ItemData->MaxStackSize, ItemInstance.StackCount));
				++CountFilledSlot;

				ItemInstance.StackCount -= Slot.Item.StackCount;
				if (ItemInstance.StackCount == 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInventoryPage::CanAddItem(FItemInstance Item) const
{
	if (!Item.ItemData->bIsStackable && !HasEmptySlot())
	{
		return false;
	}

	if (Item.ItemData->bIsStackable)
	{
		// If stackable slot exist, allocate possible max count first.
		for (const FInventorySlot& Slot : Slots)
		{
			if (!Slot.IsEmpty() && Item.ItemID == Slot.Item.ItemID)
			{
				Item.StackCount -= (Item.ItemData->MaxStackSize - Slot.Item.StackCount); 
				if (Item.StackCount == 0)
				{
					return true;
				}
			}
		}

		// place left items in empty slotss
		for (const FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Item.StackCount -= Item.ItemData->MaxStackSize;
				if (Item.StackCount <= 0)
				{
					return true;
				}
			}
		}
	}
	else
	{
		// place left items in empty slotss
		for (const FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Item.StackCount -= Item.ItemData->MaxStackSize;
				if (Item.StackCount <= 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void FInventoryPage::RemoveItem(int32 SlotIndex)
{
	Slots[SlotIndex].Item = FItemInstance();
	--CountFilledSlot;
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

int32 FInventoryPage::GetFirstEmptySlotIndex() const
{
	for (int32 Index = 0; Index < CountMaxSlot; ++Index)
	{
		if (Slots[Index].IsEmpty())
		{
			return Index;
		}
	}
	return -1;
}
