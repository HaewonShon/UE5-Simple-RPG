// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"


FItemInstance::FItemInstance()
{
	DataAsset = nullptr;
	StackCount = 0;
}

FItemInstance::FItemInstance(UItemData* Item, int32 Count)
{
	DataAsset = Item;
	ItemID = DataAsset->GetPrimaryAssetId();
	StackCount = Count;
}

bool FItemInstance::SetItem(UItemData* Item, int32 Count)
{
	if (!Item)
	{
		return false;
	}

	DataAsset = Item;
	ItemID = DataAsset->GetPrimaryAssetId();
	StackCount = Count;

	return true;
}

bool FItemInstance::AddStack(FItemInstance& OtherInstance)
{
	if (ItemID != OtherInstance.ItemID)
	{
		return false;
	}

	if (!DataAsset->bIsStackable)
	{
		return false;
	}

	int32 RemainingStackCount = DataAsset->MaxStackSize - StackCount;
	StackCount += FMath::Min(OtherInstance.StackCount, RemainingStackCount);
	OtherInstance.StackCount -= FMath::Min(OtherInstance.StackCount, RemainingStackCount);

	return true;
}

bool FItemInstance::RemoveStack(int32 Count)
{
	if (StackCount < Count)
	{
		return false;
	}

	StackCount -= Count;
	return true;
}

void UItemData::PostInitProperties()
{
	Super::PostInitProperties();
}

void UItemData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);	
	if (!bIsStackable)
	{
		MaxStackSize = 1;
	}
}

FPrimaryAssetId UItemData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("ItemData"), GetFName());
}

FItemDescription UItemData::BuildDescriptionData() const
{
	FItemDescription Description;
	Description.Name = DisplayName;
	Description.Icon = Icon.Get();
	Description.Rarity = Rarity;

	FItemDetail Detail;
	Detail.DetailText = DescriptionText;
	Description.Payload.Set<FItemDetail>(Detail);

	return Description;
}
