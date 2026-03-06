// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"
#include "Shared/Item/ItemData.h"
#include "Shared/Item/EquipmentItemData.h"
#include "GameplayEffectTypes.h"
#include "InventoryComponent.generated.h"

// Define Log Inventory for Inventory-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All)

DECLARE_DELEGATE(FInventoryContentChangedDelegate)
DECLARE_DELEGATE(FEquipmentChangedDelegate)
DECLARE_DELEGATE_OneParam(FItemCountChangedDelegate, const FPrimaryAssetId&)

UENUM()
enum class EInventoryMode : uint8
{
	Normal,
	Shop,
};

USTRUCT()
struct FEquipmentInfo
{
	GENERATED_BODY()

	FInventorySlot Slot;
	FActiveGameplayEffectHandle ActiveSpecHandle;
};

UCLASS(ClassGroup = (SimpleRPG), meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;
	void SetAbilitySystemComponentRef(UAbilitySystemComponent* ASC);

	/*
	*	Inventory Management functions
	*/
	bool CanAddItem(FItemInstance ItemInstance) const;
	bool AddItem(FItemInstance& ItemInstance);

	void RemoveItem(int32 SlotIndex, bool bShouldDropItem);

	void SwapItems(int32 Index1, int32 Index2);
	
	bool UseItem(int32 SlotIndex);

	bool CanAddRewardItems(const TArray<struct FItemReward>& RewardItems);
	bool AddRewardItems(const TArray<struct FItemReward>& RewardItems);

	/*
	*	Equipment Management functions
	*/
	void TryEquipItem(int32 SlotIndex, EEquipmentType EquipmentType);

	void TryEquipItem(int32 SlotIndex);

	void TryRemoveEquipment(EEquipmentType EquipmentType);

	FInventoryPage& GetPage();

	const FInventorySlot& GetEquipmentSlot(EEquipmentType EquipmentType) const;

	/*
	*	Request functions for UI
	*/
	FItemDescription GetItemDescription(int32 SlotIndex);
	FItemDescription GetItemDescription(EEquipmentType EquipmentType);
	int32 RequestItemCount(const FPrimaryAssetId& ItemId);

	FInventoryContentChangedDelegate OnInventoryContentChanged;
	FEquipmentChangedDelegate OnEquipmentContentChanged;
	FItemCountChangedDelegate OnItemCountChanged;

	void SetShopMode() { InventoryMode = EInventoryMode::Shop; }
	void SetNormalMode() { InventoryMode = EInventoryMode::Normal; }
	EInventoryMode GetCurrentMode() const { return InventoryMode; }
protected:
	/* Checker for equipment - category */
	bool CanEquipItem(const FItemInstance& Item, EEquipmentType EquipmentType);
	bool CanRemoveEquipment(EEquipmentType EquipmentType);
	void EquipCurrentItem(EEquipmentType EquipmentType);
	void UnequipCurrentItem(EEquipmentType EquipmentType);

	UPROPERTY()
	FInventoryPage InventoryPage;

	UPROPERTY()
	TMap<EEquipmentType, FEquipmentInfo> EquipmentSlots;

	UPROPERTY()
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponentRef;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<class UGameplayEffect> EquipmentGE;

	EInventoryMode InventoryMode;
};
