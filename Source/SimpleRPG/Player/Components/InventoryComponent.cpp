// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "World/ItemSpawn/ItemSpawnSubsystem.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Shared/Reward/Reward.h"

DEFINE_LOG_CATEGORY(LogInventory);

EInventoryCategory ConvertItemToInventoryCategory(EItemCategory ItemCategory)
{
	switch (ItemCategory)
	{
	case EItemCategory::Equipment:
		return EInventoryCategory::Equipment;
	case EItemCategory::Consumable:
		return EInventoryCategory::Consumable;
	case EItemCategory::Material:
		return EInventoryCategory::Material;
	}
	return EInventoryCategory::Count;
}

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

	for (EEquipmentType Category : TEnumRange<EEquipmentType>())
	{
		EquipmentSlots.Add({ Category, FEquipmentInfo() });
	}
}

void UInventoryComponent::SetAbilitySystemComponentRef(UAbilitySystemComponent* ASC)
{
	AbilitySystemComponentRef = ASC;
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

	OnInventoryContentChanged.Execute();
}

bool UInventoryComponent::UseItem(int32 SlotIndex)
{
	UE_LOG(LogInventory, Verbose, TEXT("UseItem SlotIndex %i requested"), SlotIndex);

	if (InventoryMode == EInventoryMode::Shop)
	{
		// Try Selling Item
	}
	
	// use otherwise..


	return false;
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

void UInventoryComponent::TryEquipItem(int32 SlotIndex, EEquipmentType EquipmentType)
{
	const FItemInstance& EquipmentItem = InventoryPage.GetItemInstance(SlotIndex);
	if (CanEquipItem(EquipmentItem, EquipmentType))
	{
		Swap(EquipmentSlots[EquipmentType].Slot, InventoryPage.Slots[SlotIndex]);
		UnequipCurrentItem(EquipmentType);
		EquipCurrentItem(EquipmentType);

		OnInventoryContentChanged.ExecuteIfBound();
		OnEquipmentContentChanged.ExecuteIfBound();
	}
}

void UInventoryComponent::TryEquipItem(int32 SlotIndex)
{
	const FItemInstance& EquipmentItem = InventoryPage.GetItemInstance(SlotIndex);
	EEquipmentType Type = Cast<UEquipmentItemData>(EquipmentItem.DataAsset)->EquipmentType;
	TryEquipItem(SlotIndex, Type);
}

void UInventoryComponent::TryRemoveEquipment(EEquipmentType EquipmentType)
{
	if (CanRemoveEquipment(EquipmentType))
	{
		UnequipCurrentItem(EquipmentType);

		Swap(EquipmentSlots[EquipmentType].Slot, InventoryPage.Slots[InventoryPage.GetFirstEmptySlotIndex()]);

		OnInventoryContentChanged.ExecuteIfBound();
		OnEquipmentContentChanged.ExecuteIfBound();
	}
}

FInventoryPage& UInventoryComponent::GetPage()
{
	return InventoryPage;
}

const FInventorySlot& UInventoryComponent::GetEquipmentSlot(EEquipmentType EquipmentType) const
{
	return EquipmentSlots[EquipmentType].Slot;
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

FItemDescription UInventoryComponent::GetItemDescription(EEquipmentType EquipmentType)
{
	const FInventorySlot& Slot = GetEquipmentSlot(EquipmentType);
	if (!Slot.IsEmpty())
	{
		return Slot.Item.DataAsset->BuildDescriptionData();
	}
	return FItemDescription();
}

int32 UInventoryComponent::RequestItemCount(const FPrimaryAssetId& ItemId)
{
	int32 Count = 0;

	if (UItemDatabaseSubsystem* ItemDB = GetWorld()->GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>())
	{
		const UItemData* Item = ItemDB->Get(ItemId);
		EInventoryCategory PageCategory = ConvertItemToInventoryCategory(Item->Category);

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

/****************************************************************************************
*
*   Equipment Related functions
* 
****************************************************************************************/

bool UInventoryComponent::CanEquipItem(const FItemInstance& Item, EEquipmentType EquipmentType)
{
	if (Item.DataAsset == nullptr)
	{
		return true;
	}

	// Test equipment type
	if (const UEquipmentItemData* Equipment = Cast<UEquipmentItemData>(Item.DataAsset))
	{
		if (Equipment->EquipmentType == EquipmentType)
		{
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::CanRemoveEquipment(EEquipmentType EquipmentType)
{
	if (InventoryPage.HasEmptySlot())
	{
		return true;
	}
	return false;
}

void UInventoryComponent::EquipCurrentItem(EEquipmentType EquipmentType)
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("AbilitySystemComponent is not set properly in InventoryComponent"));
		return;
	}

	if (!EquipmentGE)
	{
		UE_LOG(LogInventory, Warning, TEXT("Equipment GE not registered"));
		return;
	}

	// Apply Item stat to ASC
	UAbilitySystemComponent* SourceASC = AbilitySystemComponentRef.Get();
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponentRef->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());


	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(EquipmentGE, 1.0f, EffectContext);
	const FItemInstance& ItemInstance = EquipmentSlots[EquipmentType].Slot.Item;
	const UEquipmentItemData* EquipmentData = Cast<UEquipmentItemData>(ItemInstance.DataAsset);
	if (!SpecHandle.IsValid() || !EquipmentData)
	{
		UE_LOG(LogInventory, Warning, TEXT("Equipment SpecHandle is not valid"));
		return;
	}

	// Write Equipment Stats into GameplayEffectSpec
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.AttackPower"), EquipmentData->Stat.AttackPower);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Defense"), EquipmentData->Stat.Defense);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.CritChance"), EquipmentData->Stat.CritChance);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.MaxHealth"), EquipmentData->Stat.MaxHealth);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.HealthRegen"), EquipmentData->Stat.HealthRegen);
	
	SpecHandle.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Weapon.Sword"));

	FActiveGameplayEffectHandle Handle = AbilitySystemComponentRef->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	if (!Handle.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("Failed to apply equipment stats to the player"));
	}
	else
	{
		UE_LOG(LogInventory, Verbose, TEXT("Succeed to apply equipment stats to the player"));
		EquipmentSlots[EquipmentType].ActiveSpecHandle = Handle;
	}
}

void UInventoryComponent::UnequipCurrentItem(EEquipmentType EquipmentType)
{
	// Remove Item Effect using equipment slot's Active GE Handle
	FActiveGameplayEffectHandle& Handle = EquipmentSlots[EquipmentType].ActiveSpecHandle;
	if (Handle.IsValid())
	{
		if (AbilitySystemComponentRef->RemoveActiveGameplayEffect(Handle))
		{
			Handle.Invalidate();
		}
		else
		{
			UE_LOG(LogInventory, Warning, TEXT("Failed to remove equipment GE from character"));
		}
	}
}
