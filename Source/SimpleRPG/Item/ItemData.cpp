// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"

void FItemInstance::SetItem(UItemData* Item)
{
	ItemData = Item;
	ItemID = ItemData->GetPrimaryAssetId();
}
