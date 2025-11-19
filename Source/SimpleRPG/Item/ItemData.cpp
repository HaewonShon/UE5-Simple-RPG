// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"

FItemInstance::FItemInstance()
{
	ItemData = nullptr;
	StackCount = 0;
}

FItemInstance::FItemInstance(UItemData* Item, int32 Count)
{
	ItemData = Item;
	ItemID = ItemData->GetPrimaryAssetId();
	StackCount = Count;
}

bool FItemInstance::SetItem(UItemData* Item, int32 Count)
{
	if (!Item)
	{
		return false;
	}

	ItemData = Item;
	ItemID = ItemData->GetPrimaryAssetId();
	StackCount = Count;

	return true;
}

bool FItemInstance::AddStack(FItemInstance& OtherInstance)
{
	if (ItemID != OtherInstance.ItemID)
	{
		return false;
	}

	if (!ItemData->bIsStackable)
	{
		return false;
	}

	int32 RemainingStackCount = ItemData->MaxStackSize - StackCount;
	StackCount += FMath::Min(OtherInstance.StackCount, RemainingStackCount);
	OtherInstance.StackCount -= FMath::Min(OtherInstance.StackCount, RemainingStackCount);

	return true;
}
