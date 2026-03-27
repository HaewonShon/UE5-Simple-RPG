// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemData.h"
#include "Player/Components/InventoryComponent.h"

FItemInstance::FItemInstance()
{
}

FItemInstance::FItemInstance(const FItemInstance& Other)
{
	DataAsset = Other.DataAsset;
	ItemID = Other.ItemID;
	StackCount = Other.StackCount;
	EnhancementInfo = Other.EnhancementInfo;
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

FItemDescription FItemInstance::BuildDescriptionData() const
{
	if (!DataAsset.IsValid())
	{
		return FItemDescription();
	}

	FItemDescription Description = DataAsset->BuildDescriptionData();
	if (DataAsset->CanEnhance() && EnhancementInfo.EnhancementLevel > 0)
	{
		Description.Name = 
			FText::Format(FText::FromString("{0} (+ {1})"), 
				Description.Name, EnhancementInfo.EnhancementLevel);

		FItemStat TotalStat = GetTotalStat();	
		const FItemStat& EnhancedStat = EnhancementInfo.EnhancedStat;
		auto& Stats = Description.Payload.Get<FEquipmentDetail>().Stats;

		if (EnhancedStat.AttackPower != 0)
			Stats.Emplace(EStat::AttackPower, FText::Format(FText::FromString("{0} (+ {1})"),
				TotalStat.AttackPower, EnhancementInfo.EnhancedStat.AttackPower));
		if (EnhancedStat.Defense != 0)
			Stats.Emplace(EStat::Defense, FText::Format(FText::FromString("{0} (+ {1})"),
				TotalStat.Defense, EnhancementInfo.EnhancedStat.Defense));
		if (EnhancedStat.CritChance != 0)
			Stats.Emplace(EStat::CritChance, FText::Format(FText::FromString("{0} (+ {1})"),
				TotalStat.CritChance, EnhancementInfo.EnhancedStat.CritChance));
		if (EnhancedStat.MaxHealth != 0)
			Stats.Emplace(EStat::MaxHealth, FText::Format(FText::FromString("{0} (+ {1})"),
				TotalStat.MaxHealth, EnhancementInfo.EnhancedStat.MaxHealth));
		if (EnhancedStat.HealthRegen != 0)
			Stats.Emplace(EStat::HealthRegen, FText::Format(FText::FromString("{0} (+ {1})"),
				TotalStat.HealthRegen, EnhancementInfo.EnhancedStat.HealthRegen));
	}

	return Description;
}

bool UItemData::CanExecute(AActor* Executer, const FSlotAddress& Address) const
{
	if (Address.ContainerType == ESlotType::Shop)
	{
		return true;
	}
	else if (Address.ContainerType == ESlotType::Storage)
	{
		UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>();
		if (Inventory && (Inventory->GetCurrentMode() == EInventoryMode::Shop))
		{
			return true;
		}
	}
	return false;
}

// Common item execution logic
void UItemData::Execute(AActor* Executer, const FSlotAddress& Address)
{
	if (Address.ContainerType == ESlotType::Shop)
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemData::Execute from shop called"));
		if(UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>())
		{
			Inventory->RequestPurchaseItem(Address.SlotIndex);
		}
	}
	else if (Address.ContainerType == ESlotType::Storage)
	{
		UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>();
		if (Inventory && (Inventory->GetCurrentMode() == EInventoryMode::Shop))
		{
			Inventory->RequestSellItem(Address.SlotIndex);
		}
	}
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
