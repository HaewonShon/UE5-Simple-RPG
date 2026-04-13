// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Components/EquipmentComponent.h"
#include "Misc/EnumRange.h"
#include "AbilitySystemComponent.h"
#include "../SimpleRPGPlayerState.h"
#include "InventoryComponent.h"
#include "Shared/Item/ItemManagementSubsystem.h"

DEFINE_LOG_CATEGORY(LogEquipment)

EEquipmentType UEquipmentComponent::SlotToEquipmentType(ESlotType SlotType)
{
	return EEquipmentType();
}

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Create equipment slots
	for (EEquipmentType Category : TEnumRange<EEquipmentType>())
	{
		EquipmentSlots.Add({ Category, FEquipmentInfo() });
	}

	InventoryComponentRef = Cast<ASimpleRPGPlayerState>(GetOwner())->GetComponentByClass<UInventoryComponent>();
	AbilitySystemComponentRef = Cast<ASimpleRPGPlayerState>(GetOwner())->GetComponentByClass<UAbilitySystemComponent>();
	check(InventoryComponentRef.IsValid() && AbilitySystemComponentRef.IsValid());
}

FInventorySlot& UEquipmentComponent::GetEquipmentSlot(int32 Index)
{
	return EquipmentSlots[static_cast<EEquipmentType>(Index)].Slot;
}

FInventorySlot& UEquipmentComponent::GetEquipmentSlot(EEquipmentType Type)
{
	return EquipmentSlots[Type].Slot;
}

const FInventorySlot& UEquipmentComponent::GetEquipmentSlot(EEquipmentType EquipmentType) const
{
	return EquipmentSlots[EquipmentType].Slot;
}

FItemDescription UEquipmentComponent::GetItemDescription(EEquipmentType EquipmentType) const
{
	const FInventorySlot& Slot = GetEquipmentSlot(EquipmentType);
	if (!Slot.IsEmpty())
	{
		return Slot.Item.BuildDescriptionData();
	}
	return FItemDescription();
}

bool UEquipmentComponent::RequestRegisterEnhanceTarget(int32 SlotIndex)
{
	FInventorySlot& Slot = GetEquipmentSlot(static_cast<EEquipmentType>(SlotIndex));
	if (Slot.IsEmpty())
	{
		return false;
	}

	if (UItemManagementSubsystem* ItemManagementSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagementSubsystem>())
	{
		ItemManagementSubsystem->SetEnhanceTargetItem(&Slot.Item);
		ItemManagementSubsystem->OnEnhanceCompleted.AddUObject(this, &UEquipmentComponent::ReequipCurrentItem, static_cast<EEquipmentType>(SlotIndex));
		return true;
	}

	return false;
}

bool UEquipmentComponent::RequestEquip(int32 InventorySlotIndex, int32 EquipmentSlotIndex)
{
	return InventoryComponentRef->RequestEquipItem(InventorySlotIndex, EquipmentSlotIndex);
}

bool UEquipmentComponent::TryEquip(FItemInstance& Item)
{
	if (const UEquipmentItemData* EquipmentData = Cast<UEquipmentItemData>(Item.DataAsset))
	{
		UnequipCurrentItem(EquipmentData->EquipmentType);
		Swap(Item, EquipmentSlots[EquipmentData->EquipmentType].Slot.Item);
		EquipCurrentItem(EquipmentData->EquipmentType);
		OnEquipmentContentChanged.ExecuteIfBound();
	} 
	
	return true;
}

bool UEquipmentComponent::TryEquip(int32 TargetIndex, FItemInstance& Item)
{
	return TryEquip(static_cast<EEquipmentType>(TargetIndex), Item);
}

bool UEquipmentComponent::TryEquip(EEquipmentType Type, FItemInstance& Item)
{
	if (const UEquipmentItemData* EquipmentData = Cast<UEquipmentItemData>(Item.DataAsset))
	{
		if (EquipmentData->EquipmentType == Type)
		{
			return TryEquip(Item);
		}
	}
	return false;
}

bool UEquipmentComponent::RequestRemoveEquipment(int32 Index, int32 TargetInventoryIndex)
{
	EEquipmentType EquipmentType = static_cast<EEquipmentType>(Index);
	if (CanRemoveEquipment(EquipmentType))
	{
		UnequipCurrentItem(EquipmentType);
		if (TargetInventoryIndex == -1)
		{
			if (InventoryComponentRef->AddItem(EquipmentSlots[EquipmentType].Slot.Item))
			{
				EquipmentSlots[EquipmentType].Slot.Item = FItemInstance();
				OnEquipmentContentChanged.ExecuteIfBound();
				return true;
			}
		}
		else
		{
			if (InventoryComponentRef->AddItem(EquipmentSlots[EquipmentType].Slot.Item, TargetInventoryIndex))
			{
				EquipmentSlots[EquipmentType].Slot.Item = FItemInstance();
				OnEquipmentContentChanged.ExecuteIfBound();
				return true;
			}
		}

	}
	// re-equip if failed to add item to inventory
	EquipCurrentItem(EquipmentType);
	return false;
}

bool UEquipmentComponent::CanEquipItem(const FItemInstance& Item, EEquipmentType EquipmentType)
{
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

bool UEquipmentComponent::CanRemoveEquipment(EEquipmentType EquipmentType)
{
	if (InventoryComponentRef->CanAddItem(EquipmentSlots[EquipmentType].Slot.Item))
	{
		return true;
	}
	return false;
}

void UEquipmentComponent::EquipCurrentItem(EEquipmentType EquipmentType)
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		UE_LOG(LogInventory, Warning, TEXT("AbilitySystemComponent is not set properly in InventoryComponent"));
		return;
	}

	if (!EquipmentGE)
	{
		UE_LOG(LogEquipment, Warning, TEXT("Equipment GE not registered"));
		return;
	}

	// Apply Item stat to ASC
	UAbilitySystemComponent* SourceASC = AbilitySystemComponentRef.Get();
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponentRef->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(EquipmentGE, 1.0f, EffectContext);
	const FItemInstance& ItemInstance = EquipmentSlots[EquipmentType].Slot.Item;
	if (!SpecHandle.IsValid() || !ItemInstance.IsValid())
	{
		UE_LOG(LogEquipment, Warning, TEXT("Equipment SpecHandle is not valid"));
		return;
	}

	FItemStat EquipmentItemStat = ItemInstance.GetTotalStat();
	// Write Equipment Stats into GameplayEffectSpec
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.AttackPower"), EquipmentItemStat.AttackPower);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Defense"), EquipmentItemStat.Defense);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.CritChance"), EquipmentItemStat.CritChance);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.MaxHealth"), EquipmentItemStat.MaxHealth);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.HealthRegen"), EquipmentItemStat.HealthRegen);

	SpecHandle.Data->DynamicGrantedTags.AddTag(FGameplayTag::RequestGameplayTag("Weapon.Sword"));

	FActiveGameplayEffectHandle Handle = AbilitySystemComponentRef->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	if (!Handle.IsValid())
	{
		UE_LOG(LogEquipment, Warning, TEXT("Failed to apply equipment stats to the player"));
	}
	else
	{
		UE_LOG(LogEquipment, Verbose, TEXT("Succeed to apply equipment stats to the player"));
		EquipmentSlots[EquipmentType].ActiveSpecHandle = Handle;
	}
}

void UEquipmentComponent::UnequipCurrentItem(EEquipmentType EquipmentType)
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
			UE_LOG(LogEquipment, Warning, TEXT("Failed to remove equipment GE from character"));
		}
	}
}

void UEquipmentComponent::ReequipCurrentItem(EEnhanceResult EnhanceResult, EEquipmentType EquipmentType)
{
	if (EnhanceResult == EEnhanceResult::Success)
	{
		UnequipCurrentItem(EquipmentType);
		EquipCurrentItem(EquipmentType);
	}
}
