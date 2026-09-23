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

int32 FInventoryPage::GetItemAmountById(const FPrimaryAssetId& ItemId) const
{
	int32 Amount = 0;
	for (const FInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty() && Slot.Item.ItemID == ItemId)
		{
			Amount += Slot.Item.Amount;
		}
	}

	return Amount;
}

bool FInventoryPage::AddItem(FItemInstance& ItemInstance)
{
	if (!ItemInstance.DataAsset->bIsStackable && !HasEmptySlot())
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory does not have empty slot to add item"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("AddItem GivenID: %s"), *ItemInstance.ItemID.ToString());

	if (ItemInstance.DataAsset->bIsStackable)
	{
		// If stackable slot exist, place it first.
		for (FInventorySlot& Slot : Slots)
		{
			if (!Slot.IsEmpty() && ItemInstance.ItemID == Slot.Item.ItemID)
			{
				UE_LOG(LogTemp, Warning, TEXT("AddItem SelectedSlot: %s"), *Slot.Item.ItemID.ToString());
				Slot.Item.AddStack(ItemInstance);
				if (ItemInstance.Amount == 0)

					return true;
			}
		}


		// place left items in empty slotss
		for (FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.Item.SplitFrom(ItemInstance, FMath::Min(ItemInstance.DataAsset->MaxStackSize, ItemInstance.Amount));
				++CountFilledSlot;
				if (ItemInstance.Amount <= 0)
				{
					return true;
				}
			}
		}
	}
	else
	{
		check(ItemInstance.IsValid());

		// place left items in empty slots
		for (FInventorySlot& Slot : Slots)
		{
			if (Slot.IsEmpty())
			{
				Slot.Item.SplitFrom(ItemInstance, FMath::Min(ItemInstance.DataAsset->MaxStackSize, ItemInstance.Amount));
				++CountFilledSlot;
				if (ItemInstance.Amount <= 0)
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
				Item.Amount -= (Item.DataAsset->MaxStackSize - Slot.Item.Amount);
				if (Item.Amount == 0)
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
				Item.Amount -= Item.DataAsset->MaxStackSize;
				if (Item.Amount <= 0)
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
				Item.Amount -= Item.DataAsset->MaxStackSize;
				if (Item.Amount <= 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}


bool FInventoryPage::CanRemoveItem(const FPrimaryAssetId& ItemId, int32 Amount) const
{
	if (GetItemAmountById(ItemId) < Amount)
	{
		return false;
	}
	return true;
}

bool FInventoryPage::RemoveItem(int32 SlotIndex, int32 Amount)
{
	FItemInstance& Item = Slots[SlotIndex].Item;
	if (Item.RemoveStack(Amount))
	{
		if (Item.Amount == 0)
		{
			Slots[SlotIndex].Item = FItemInstance();
			--CountFilledSlot;
		}
		return true;
	}
	return false;
}

bool FInventoryPage::RemoveItem(const FPrimaryAssetId& TargetId, int32 Amount)
{
	if (!CanRemoveItem(TargetId, Amount))
	{
		return false;
	}

	for (FInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty() && Slot.Item.ItemID == TargetId)
		{
			const int32 RemoveAmount = FMath::Min(Slot.Item.Amount, Amount);
			Slot.Item.Amount -= RemoveAmount;
			Amount -= RemoveAmount;

			if (Slot.Item.Amount <= 0)
			{
				Slot.Item = FItemInstance();
				--CountFilledSlot;
			}

			if (Amount == 0)
			{
				break;
			}
		}
	}

	ensure(Amount == 0);
	return true;
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
