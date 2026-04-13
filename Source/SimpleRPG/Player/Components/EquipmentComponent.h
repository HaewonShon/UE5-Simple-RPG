// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shared/Item/EquipmentItemData.h"
#include "Inventory.h"
#include "GameplayEffectTypes.h"
#include "Shared/Item/ItemManagementSubsystem.h"
#include "EquipmentComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All)

DECLARE_DELEGATE(FEquipmentChangedDelegate)

USTRUCT()
struct FEquipmentInfo
{
	GENERATED_BODY()

	FInventorySlot Slot;
	FActiveGameplayEffectHandle ActiveSpecHandle;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	static EEquipmentType SlotToEquipmentType(ESlotType SlotType);

	// Sets default values for this component's properties
	UEquipmentComponent();
	FInventorySlot& GetEquipmentSlot(int32 Index);
	FInventorySlot& GetEquipmentSlot(EEquipmentType Type);
	
	/************************************************
	*	Equipment Management functions
	************************************************/
	/*** Inventory -> Equipment ***/
	bool RequestEquip(int32 InventorySlotIndex, int32 EquipmentSlotIndex);
	bool TryEquip(FItemInstance& Item);
	bool TryEquip(int32 TargetIndex, FItemInstance& Item);
	bool TryEquip(EEquipmentType Type, FItemInstance& Item);
	bool RequestRemoveEquipment(int32 Index, int32 TargetInventoryIndex = -1);

	const FInventorySlot& GetEquipmentSlot(EEquipmentType EquipmentType) const;
	FItemDescription GetItemDescription(EEquipmentType EquipmentType) const;

	FEquipmentChangedDelegate OnEquipmentContentChanged;

	bool RequestRegisterEnhanceTarget(int32 SlotIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* Checker for equipment - category */
	bool CanEquipItem(const FItemInstance& Item, EEquipmentType EquipmentType);
	bool CanRemoveEquipment(EEquipmentType EquipmentType);
	void EquipCurrentItem(EEquipmentType EquipmentType);
	void UnequipCurrentItem(EEquipmentType EquipmentType);
	void ReequipCurrentItem(EEnhanceResult EnhanceResult, EEquipmentType EquipmentType);

	UPROPERTY()
	TMap<EEquipmentType, FEquipmentInfo> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<class UGameplayEffect> EquipmentGE;

	UPROPERTY()
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponentRef;

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> InventoryComponentRef;
};
