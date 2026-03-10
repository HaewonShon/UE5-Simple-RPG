// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"
#include "../SimpleRPGPlayerState.h"
#include "World/ItemSpawn/ItemSpawnSubsystem.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Shared/Reward/Reward.h"
#include "EquipmentComponent.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryMode = EInventoryMode::Normal;
	const int32 SlotCountPerPage = 8 * 8;
	InventoryPage = FInventoryPage(SlotCountPerPage);

	EquipmentComponentRef = Cast<ASimpleRPGPlayerState>(GetOwner())->GetComponentByClass<UEquipmentComponent>();
	check(EquipmentComponentRef.IsValid());
}

bool UInventoryComponent::CanAddItem(FItemInstance ItemInstance) const
{
	if (!ItemInstance.DataAsset.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("AddItem: Item is not valid"));
		return false;
	}

	return InventoryPage.CanAddItem(ItemInstance);
}

bool UInventoryComponent::AddItem(FItemInstance& ItemInstance)
{
	if (!ItemInstance.DataAsset.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("AddItem: Item is not valid"));
		return false;
	}

	if (InventoryPage.AddItem(ItemInstance))
	{
		OnInventoryContentChanged.ExecuteIfBound();
		OnItemCountChanged.ExecuteIfBound(ItemInstance.ItemID);
		return true;
	}
	
	return false;
}

bool UInventoryComponent::AddItem(FItemInstance& ItemInstance, int32 SlotIndex)
{
	if (!InventoryPage.IsSlotEmpty(SlotIndex))
	{
		return false;
	}

	InventoryPage.Slots[SlotIndex].Item = ItemInstance;
	return true;
}

void UInventoryComponent::RemoveItem(int32 SlotIndex, bool bShouldDropItem)
{
	if (InventoryPage.IsSlotEmpty(SlotIndex))
	{
		return;
	}

	if (bShouldDropItem)
	{
		const FItemInstance& ItemInstance = InventoryPage.GetItemInstance(SlotIndex);

		// spawn actor
		if (UItemSpawnSubsystem* ItemSpawnSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemSpawnSubsystem>())
		{
			if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
			{
				UE_LOG(LogInventory, Verbose, TEXT("Item spawn %i requested"), SlotIndex);
				ItemSpawnSubsystem->SpawnItem(ItemInstance, PlayerState->GetPawn()->GetActorLocation());
			}
		}
	}

	FPrimaryAssetId RemovedItemId = InventoryPage.Slots[SlotIndex].Item.ItemID;
	InventoryPage.RemoveItem(SlotIndex);

	UE_LOG(LogInventory, Verbose, TEXT("Item %s Removed"), *RemovedItemId.ToString());
	OnInventoryContentChanged.ExecuteIfBound();
	OnItemCountChanged.ExecuteIfBound(RemovedItemId);	
}

void UInventoryComponent::SwapItems(int32 Index1, int32 Index2)
{
	InventoryPage.SwapItems(Index1, Index2);
	OnInventoryContentChanged.ExecuteIfBound();
}

bool UInventoryComponent::CanAddRewardItems(const TArray<FItemReward>& RewardItems)
{
	bool bResult = true;
	int32 RequiredSlot = 0;

	UItemDatabaseSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>();
	check(ItemDB);

	for (const FItemReward& Reward : RewardItems)
	{
		const UItemData* ItemData = ItemDB->Get(Reward.ItemId);
		if (ItemData->bIsStackable)
		{
			// TODO : consider existing slot first
			RequiredSlot += (Reward.Amount + ItemData->MaxStackSize - 1) / ItemData->MaxStackSize;
		}
		else
		{
			RequiredSlot += Reward.Amount;
		}
	}

	int32 RemainingSlot = InventoryPage.CountMaxSlot - InventoryPage.CountFilledSlot;
	if (RemainingSlot < RequiredSlot)
	{
		bResult = false;
	}

	return true;
}

bool UInventoryComponent::AddRewardItems(const TArray<struct FItemReward>& RewardItems)
{
	UItemDatabaseSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>();
	check(ItemDB);

	for (const FItemReward& Reward : RewardItems)
	{
		UItemData* ItemData = ItemDB->Get(Reward.ItemId);

		if (ItemData->bIsStackable)
		{
			FItemInstance RewardItemInstance;
			RewardItemInstance.SetItem(ItemData, Reward.Amount);

			AddItem(RewardItemInstance);
		}
		else
		{
			for (int32 i = 0; i < Reward.Amount; ++i)
			{
				FItemInstance RewardItemInstance;
				RewardItemInstance.SetItem(ItemData);

				AddItem(RewardItemInstance);
			}
		}
	}

	return true;
}

FInventoryPage& UInventoryComponent::GetPage()
{
	return InventoryPage;
}

bool UInventoryComponent::RequestEquipItem(int32 SlotIndex)
{
	if (EquipmentComponentRef->TryEquip(InventoryPage.Slots[SlotIndex].Item))
	{
		OnInventoryContentChanged.ExecuteIfBound();
		return true;
	}
	return false;
}

bool UInventoryComponent::RequestEquipItem(int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (EquipmentComponentRef->TryEquip(TargetSlotIndex, InventoryPage.Slots[SourceSlotIndex].Item))
	{
		OnInventoryContentChanged.ExecuteIfBound();		
		return true;
	}
	return false;
}

bool UInventoryComponent::RequestRemoveEquipment(int32 EquipmentIndex, int32 TargetSlotIndex)
{
	if (InventoryPage.IsSlotEmpty(TargetSlotIndex))
	{
		// Equipment Á¦°Å ÈÄ 
		return EquipmentComponentRef->RequestRemoveEquipment(EquipmentIndex, TargetSlotIndex);
	}
	else
	{
		if (EquipmentComponentRef->TryEquip(EquipmentIndex, InventoryPage.Slots[TargetSlotIndex].Item))
		{
			OnInventoryContentChanged.ExecuteIfBound();
			return true;
		}
	}
	return false;
}

FItemDescription UInventoryComponent::GetItemDescription(int32 SlotIndex)
{
	const FInventorySlot& Slot = InventoryPage.Slots[SlotIndex];
	if (!Slot.IsEmpty())
	{
		FItemDescription Description = Slot.Item.DataAsset->BuildDescriptionData();
		if(GetCurrentMode() == EInventoryMode::Shop)
		{ 
			Description.Price = FText::Format(FText::FromString(TEXT("Sell price: {0}G")), Slot.Item.DataAsset->SellPrice);
		}
		return Description;
	}
	return FItemDescription();
}

int32 UInventoryComponent::RequestItemCount(const FPrimaryAssetId& ItemId)
{
	int32 Count = 0;

	if (UItemDatabaseSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>())
	{
		const UItemData* Item = ItemDB->Get(ItemId);
		for (const FInventorySlot& Slot : InventoryPage.Slots)
		{
			if (!Slot.IsEmpty() && Slot.Item.ItemID == ItemId)
			{
				Count += Slot.Item.StackCount;
			}
		}
	}

	return Count;
}