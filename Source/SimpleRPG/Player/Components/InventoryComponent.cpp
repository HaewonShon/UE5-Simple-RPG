// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"
#include "../SimpleRPGPlayerState.h"
#include "World/ItemSpawn/ItemSpawnSubsystem.h"
#include "Shared/Item/ItemManagementSubsystem.h"
#include "Shared/Reward/Reward.h"
#include "EquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "Interaction/Shop/ShopComponent.h"
#include "Shared/Item/ConsumableItemData.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}

bool UInventoryComponent::RequestUseItem(int32 SlotIndex)
{
	if (InventoryMode == EInventoryMode::Shop)
	{
		return RequestSellItem(SlotIndex);
	}

	const UConsumableItemData* Item = Cast<UConsumableItemData>(InventoryPage.Slots[SlotIndex].Item.DataAsset);
	if (!Item)
	{
		return false;
	}

	return TryUseItem(SlotIndex);
}
 
bool UInventoryComponent::RequestSellItem(int32 SlotIndex)
{
	if (!InteractingShopComponentRef.IsValid())
	{
		return false;
	}

	InteractingShopComponentRef->RequestSellItem(SlotIndex);
	return true;
}

bool UInventoryComponent::RequestPurchaseItem(int32 ShopSlotIndex)
{
	if (!InteractingShopComponentRef.IsValid())
	{
		return false;
	}

	InteractingShopComponentRef->RequestPurchaseItem(ShopSlotIndex);
	return true; // TODO : purchase temp return
}

bool UInventoryComponent::RequestRegisterEnhanceTarget(int32 SlotIndex)
{
	if (InventoryPage.Slots[SlotIndex].IsEmpty())
	{
		return false;
	}

	if (UItemManagementSubsystem* ItemManagementSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagementSubsystem>())
	{
		ItemManagementSubsystem->SetEnhanceTargetItem(&InventoryPage.Slots[SlotIndex].Item);
		return true;
	}

	return false;
}

void UInventoryComponent::SetShopMode(UShopComponent* ShopCmopRef)
{
	InventoryMode = EInventoryMode::Shop;
	InteractingShopComponentRef = ShopCmopRef;

	if (!InteractingShopComponentRef.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("Given shop component is not valid!"));
		SetNormalMode();
	}
}

void UInventoryComponent::SetEnhanceMode()
{
	InventoryMode = EInventoryMode::Enhancement;
}

void UInventoryComponent::SetNormalMode()
{
	InventoryMode = EInventoryMode::Normal;
	InteractingShopComponentRef = nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryMode = EInventoryMode::Normal;
	const int32 SlotCountPerPage = 8 * 8;
	InventoryPage = FInventoryPage(SlotCountPerPage);

	EquipmentComponentRef = Cast<ASimpleRPGPlayerState>(GetOwner())->GetComponentByClass<UEquipmentComponent>();
	AbilitySystemComponentRef = Cast<ASimpleRPGPlayerState>(GetOwner())->GetComponentByClass<UAbilitySystemComponent>();
	check(EquipmentComponentRef.IsValid() && AbilitySystemComponentRef.IsValid());
}

bool UInventoryComponent::TryUseItem(int32 SlotIndex)
{
	FItemInstance& ItemInstance = InventoryPage.Slots[SlotIndex].Item;
	UConsumableItemData* ItemData = Cast<UConsumableItemData>(ItemInstance.DataAsset);

	if (!ItemData || ItemInstance.Amount <= 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("consumable data class or amount is not valid"));
		return false;
	}

	// Apply Item stat to ASC
	UAbilitySystemComponent* SourceASC = AbilitySystemComponentRef.Get();
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponentRef->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());

	TSubclassOf<UGameplayEffect> GEClass = ItemData->GetGameplayEffectClass();
	if (!GEClass)
	{
		UE_LOG(LogInventory, Warning, TEXT("consumable GEClass is not valid"));
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GEClass, 1.0f, EffectContext);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("consumable specHandle is not valid"));
		return false;
	}

	if (!ItemData->SetGameplayEffectSpecHandleData(SpecHandle))
	{
		UE_LOG(LogInventory, Warning, TEXT("Failed to set consumbale GESpecHandleData"));
		return false;
	}

	FActiveGameplayEffectHandle Handle = AbilitySystemComponentRef->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	if (!Handle.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("Failed to apply consumable item to the player"));
		return false;
	}
	else
	{
		UE_LOG(LogInventory, Verbose, TEXT("Succeed to apply consumable item to the player"));
		InventoryPage.RemoveItem(SlotIndex, 1);
		OnInventoryContentChanged.Broadcast();
		OnItemCountChanged.Broadcast(ItemData->GetPrimaryAssetId());

		return true;
	}
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
		OnInventoryContentChanged.Broadcast();
		OnItemCountChanged.Broadcast(ItemInstance.ItemID);
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

bool UInventoryComponent::RemoveItem(int32 SlotIndex, bool bShouldDropItem)
{
	if (InventoryPage.IsSlotEmpty(SlotIndex))
	{
		return false;
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
	OnInventoryContentChanged.Broadcast();
	OnItemCountChanged.Broadcast(RemovedItemId);
	return true;
}

bool UInventoryComponent::RemoveItem(int32 SlotIndex, int32 Count)
{
	if (InventoryPage.IsSlotEmpty(SlotIndex))
	{
		return false;
	}

	FPrimaryAssetId ItemId = InventoryPage.Slots[SlotIndex].Item.ItemID;
	if (InventoryPage.RemoveItem(SlotIndex, Count))
	{
		OnInventoryContentChanged.Broadcast();
		OnItemCountChanged.Broadcast(ItemId);
		return true;
	}
	return false;
}

void UInventoryComponent::SwapItems(int32 Index1, int32 Index2)
{
	if (InventoryMode != EInventoryMode::Normal)
	{
		return;
	}

	InventoryPage.SwapItems(Index1, Index2);
	OnInventoryContentChanged.Broadcast();
}

bool UInventoryComponent::CanAddRewardItems(const TArray<FItemReward>& RewardItems)
{
	bool bResult = true;
	int32 RequiredSlot = 0;

	UItemManagementSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagementSubsystem>();
	check(ItemDB);

	for (const FItemReward& Reward : RewardItems)
	{
		const UItemData* ItemData = ItemDB->GetItemData(Reward.ItemId);
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
	UItemManagementSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagementSubsystem>();
	check(ItemDB);

	for (const FItemReward& Reward : RewardItems)
	{
		UItemData* ItemData = ItemDB->GetItemData(Reward.ItemId);

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
	if(InventoryMode == EInventoryMode::Shop)
	{
		return RequestSellItem(SlotIndex);
	}

	if (InventoryMode == EInventoryMode::Enhancement)
	{
		return RequestRegisterEnhanceTarget(SlotIndex);
	}

	if (EquipmentComponentRef->TryEquip(InventoryPage.Slots[SlotIndex].Item))
	{
		OnInventoryContentChanged.Broadcast();
		return true;
	}
	return false;
}

bool UInventoryComponent::RequestEquipItem(int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (EquipmentComponentRef->TryEquip(TargetSlotIndex, InventoryPage.Slots[SourceSlotIndex].Item))
	{
		OnInventoryContentChanged.Broadcast();
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
			OnInventoryContentChanged.Broadcast();
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
		FItemDescription Description = Slot.Item.BuildDescriptionData();
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

	if (UItemManagementSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagementSubsystem>())
	{
		const UItemData* Item = ItemDB->GetItemData(ItemId);
		for (const FInventorySlot& Slot : InventoryPage.Slots)
		{
			if (!Slot.IsEmpty() && Slot.Item.ItemID == ItemId)
			{
				Count += Slot.Item.Amount;
			}
		}
	}

	return Count;
}

bool UInventoryComponent::TryRemoveItem(const FPrimaryAssetId& ItemId, int32 Amount)
{
	int32 OwningAmount = RequestItemCount(ItemId);
	if (OwningAmount < Amount)
	{
		return false;
	}

	if (InventoryPage.RemoveItem(ItemId, Amount))
	{
		OnInventoryContentChanged.Broadcast();
		OnItemCountChanged.Broadcast(ItemId);
		return true;
	}
	return false;
}