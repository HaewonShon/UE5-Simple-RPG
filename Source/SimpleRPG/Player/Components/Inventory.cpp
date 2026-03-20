// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

bool FInventorySlot::IsEmpty() const
{
	return !Item.DataAsset.IsValid();
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
	if (!ItemInstance.DataAsset->bIsStackable && !HasEmptySlot())
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item"));
		return false;
	}

	if (ItemInstance.DataAsset->bIsStackable)
	{
		// If stackable slot exist, place it first.
		for (FInventorySlot& Slot : Slots)
		{
			if (!Slot.IsEmpty() && ItemInstance.ItemID == Slot.Item.ItemID)
			{
				Slot.Item.AddStack(ItemInstance);
				if (ItemInstance.StackCount == 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("stackcount: %i"), Slot.Item.StackCount);
					return true;
				} 
			}
		}

		// place left items in empty slotss
		for (FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.Item.SetItem(ItemInstance.DataAsset.Get(), FMath::Min(ItemInstance.DataAsset->MaxStackSize, ItemInstance.StackCount));
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
				Slot.Item.SetItem(ItemInstance.DataAsset.Get(),
					FMath::Min(ItemInstance.DataAsset->MaxStackSize, ItemInstance.StackCount));
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
	if (!Item.DataAsset->bIsStackable && !HasEmptySlot())
	{
		return false;
	}

	if (Item.DataAsset->bIsStackable)
	{
		// If stackable slot exist, allocate possible max count first.
		for (const FInventorySlot& Slot : Slots)
		{
			if (!Slot.IsEmpty() && Item.ItemID == Slot.Item.ItemID)
			{
				Item.StackCount -= (Item.DataAsset->MaxStackSize - Slot.Item.StackCount);
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
				Item.StackCount -= Item.DataAsset->MaxStackSize;
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
				Item.StackCount -= Item.DataAsset->MaxStackSize;
				if (Item.StackCount <= 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool FInventoryPage::RemoveItem(int32 SlotIndex)
{
	Slots[SlotIndex].Item = FItemInstance();
	--CountFilledSlot;
	return true;
}

bool FInventoryPage::RemoveItem(int32 SlotIndex, int32 Count)
{
	FItemInstance& Item = Slots[SlotIndex].Item;
	if (Item.RemoveStack(Count))
	{
		if (Item.StackCount == 0)
		{
			RemoveItem(SlotIndex);
		}
		return true;
	}
	return false;
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
