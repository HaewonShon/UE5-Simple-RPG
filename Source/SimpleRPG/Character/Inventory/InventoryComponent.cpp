// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Misc/EnumRange.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "../../Item/ItemLootSubsystem.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	//UE_LOG(LogInventory, Log, TEXT("Inventory Init"));
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	const int32 SlotCountPerPage = 24;

	// Page & Equipment slots init
	for (EInventoryCategory Category : TEnumRange<EInventoryCategory>())
	{
		InventoryPages.Add({ Category, FInventoryPage(Category, SlotCountPerPage) });
	}

	for (EEquipmentType Category : TEnumRange<EEquipmentType>())
	{
		EquipmentSlots.Add({ Category, FEquipmentInfo() });
	}

	UE_LOG(LogInventory, Verbose, TEXT("Initialized inventory pages size: %i"), InventoryPages.Num());
}

void UInventoryComponent::SetAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
	AbilitySystemComponent = ASC;
}

bool UInventoryComponent::AddItem(FItemInstance& ItemInstance)
{
	if (!ItemInstance.ItemData)
	{
		UE_LOG(LogInventory, Warning, TEXT("AddItem: Item is not valid"));
		return false;
	}

	EItemCategory ItemCategory = ItemInstance.ItemData->Category;
	EInventoryCategory PageCategory = EInventoryCategory::Count;
	switch (ItemCategory)
	{
	case EItemCategory::Equipment:
		PageCategory = EInventoryCategory::Equipment;
		break;
	case EItemCategory::Consumable:
		PageCategory = EInventoryCategory::Consumable;
		break;
	case EItemCategory::Material:
		PageCategory = EInventoryCategory::Material;
		break;
	}
	
	check(PageCategory != EInventoryCategory::Count);

	FInventoryPage& TargetPage = InventoryPages[PageCategory];
	if (TargetPage.AddItem(ItemInstance) && OnInventoryContentChanged.IsBound())
	{
		OnInventoryContentChanged.Execute(PageCategory);
		return true;
	}
	
	return false;
}

void UInventoryComponent::RemoveItem(EInventoryCategory PageCategory, int32 SlotIndex, bool bShouldDropItem)
{
	FInventoryPage& TargetPage = InventoryPages[PageCategory];
	if (TargetPage.IsSlotEmpty(SlotIndex))
	{
		return;
	}

	if (bShouldDropItem)
	{
		const FItemInstance& ItemInstance = TargetPage.GetItemInstance(SlotIndex);

		// spawn actor
		if (UItemLootSubsystem* ItemLootSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemLootSubsystem>())
		{
			if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
			{
				UE_LOG(LogInventory, Log, TEXT("Item spawn %i requested"), SlotIndex);
				ItemLootSubsystem->SpawnItem(ItemInstance, PlayerState->GetPawn()->GetActorLocation());
			}
		}
	}

	UE_LOG(LogInventory, Log, TEXT("Item %i Removed"), SlotIndex);
	TargetPage.RemoveItem(SlotIndex);
	OnInventoryContentChanged.Execute(PageCategory);
}

void UInventoryComponent::SwapItems(EInventoryCategory PageCategory, int32 Index1, int32 Index2)
{
	FInventoryPage& CurrentPage = InventoryPages[PageCategory];
	CurrentPage.SwapItems(Index1, Index2);

	OnInventoryContentChanged.Execute(PageCategory);
}

bool UInventoryComponent::UseItem(EInventoryCategory PageCategory, int32 SlotIndex)
{
	UE_LOG(LogInventory, Log, TEXT("UseItem SlotIndex %i"), SlotIndex);
	return false;
}

void UInventoryComponent::TryEquipItem(int32 SlotIndex, EEquipmentType EquipmentType)
{
	FInventoryPage& EquipmentPage = InventoryPages[EInventoryCategory::Equipment];
	const FItemInstance& EquipmentItem = EquipmentPage.GetItemInstance(SlotIndex);
	if (CanEquipItem(EquipmentItem, EquipmentType))
	{
		Swap(EquipmentSlots[EquipmentType].Slot, EquipmentPage.Slots[SlotIndex]);
		UnequipCurrentItem(EquipmentType);
		EquipCurrentItem(EquipmentType);

		OnInventoryContentChanged.ExecuteIfBound(EInventoryCategory::Equipment);
		OnEquipmentContentChanged.ExecuteIfBound();
	}
}

void UInventoryComponent::TryEquipItem(int32 SlotIndex)
{
	FInventoryPage& EquipmentPage = InventoryPages[EInventoryCategory::Equipment];
	const FItemInstance& EquipmentItem = EquipmentPage.GetItemInstance(SlotIndex);
	EEquipmentType Type = Cast<UEquipmentItemData>(EquipmentItem.ItemData)->EquipmentType;
	TryEquipItem(SlotIndex, Type);
}

void UInventoryComponent::TryRemoveEquipment(EEquipmentType EquipmentType)
{
	if (CanRemoveEquipment(EquipmentType))
	{
		UnequipCurrentItem(EquipmentType);

		FInventoryPage& EquipmentPage = InventoryPages[EInventoryCategory::Equipment];
		Swap(EquipmentSlots[EquipmentType].Slot, EquipmentPage.Slots[EquipmentPage.GetFirstEmptySlotIndex()]);

		OnInventoryContentChanged.ExecuteIfBound(EInventoryCategory::Equipment);
		OnEquipmentContentChanged.ExecuteIfBound();
	}
}

const FInventoryPage& UInventoryComponent::GetPage(EInventoryCategory PageCategory) const
{
	return InventoryPages[PageCategory];
}

const FInventorySlot& UInventoryComponent::GetEquipmentSlot(EEquipmentType EquipmentType) const
{
	return EquipmentSlots[EquipmentType].Slot;
}

FItemDescription UInventoryComponent::GetItemDescription(EInventoryCategory PageCategory, int32 SlotIndex)
{
	const FInventorySlot& Slot = GetPage(PageCategory).Slots[SlotIndex];
	if (!Slot.IsEmpty())
	{
		return Slot.Item.ItemData->BuildDescriptionData();
	}
	return FItemDescription();
}

FItemDescription UInventoryComponent::GetItemDescription(EEquipmentType EquipmentType)
{
	const FInventorySlot& Slot = GetEquipmentSlot(EquipmentType);
	if (!Slot.IsEmpty())
	{
		return Slot.Item.ItemData->BuildDescriptionData();
	}
	return FItemDescription();
}

bool UInventoryComponent::CanEquipItem(const FItemInstance& Item, EEquipmentType EquipmentType)
{
	if (Item.ItemData == nullptr)
	{
		return true;
	}

	// Test equipment type
	if (const UEquipmentItemData* Equipment = Cast<UEquipmentItemData>(Item.ItemData))
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
	if (InventoryPages[EInventoryCategory::Equipment].HasEmptySlot())
	{
		return true;
	}
	return false;
}

void UInventoryComponent::EquipCurrentItem(EEquipmentType EquipmentType)
{
	if (!AbilitySystemComponent.IsValid())
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
	UAbilitySystemComponent* SourceASC = AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());


	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(EquipmentGE, 1.0f, EffectContext);
	const FItemInstance& ItemInstance = EquipmentSlots[EquipmentType].Slot.Item;
	const UEquipmentItemData* EquipmentData = Cast<UEquipmentItemData>(ItemInstance.ItemData);
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

	FActiveGameplayEffectHandle Handle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	if (!Handle.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("Failed to apply equipment stats to the player"));
	}
	else
	{
		UE_LOG(LogInventory, Log, TEXT("Succeed to apply equipment stats to the player"));
		EquipmentSlots[EquipmentType].ActiveSpecHandle = Handle;

		FGameplayTagContainer OwnedTags;
		SourceASC->GetOwnedGameplayTags(OwnedTags);

		for (const FGameplayTag& Tag : OwnedTags)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASC Tag: %s"), *Tag.ToString());
		}
		UE_LOG(LogTemp, Warning, TEXT("ASC owner: %s"), *SourceASC->GetOwner()->GetName());
	}
}

void UInventoryComponent::UnequipCurrentItem(EEquipmentType EquipmentType)
{
	// Remove Item Effect using equipment slot's Active GE Handle
	FActiveGameplayEffectHandle& Handle = EquipmentSlots[EquipmentType].ActiveSpecHandle;
	if (Handle.IsValid())
	{
		if (AbilitySystemComponent->RemoveActiveGameplayEffect(Handle))
		{
			Handle.Invalidate();
		}
		else
		{
			UE_LOG(LogInventory, Warning, TEXT("Failed to remove equipment GE from character"));
		}
	}
}
