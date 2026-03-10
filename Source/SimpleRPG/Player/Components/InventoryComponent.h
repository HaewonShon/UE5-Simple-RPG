// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"
#include "Shared/Item/ItemData.h"
#include "InventoryComponent.generated.h"

// Define Log Inventory for Inventory-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All)

DECLARE_DELEGATE(FInventoryContentChangedDelegate)
DECLARE_DELEGATE_OneParam(FItemCountChangedDelegate, const FPrimaryAssetId&)

UENUM()
enum class EInventoryMode : uint8
{
	Normal,
	Shop,
};

UCLASS(ClassGroup = (SimpleRPG), meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	/*
	*	Inventory Management functions
	*/
	bool CanAddItem(FItemInstance ItemInstance) const;
	bool AddItem(FItemInstance& ItemInstance);
	bool AddItem(FItemInstance& ItemInstance, int32 SlotIndex);
	void RemoveItem(int32 SlotIndex, bool bShouldDropItem);

	void SwapItems(int32 Index1, int32 Index2);

	bool CanAddRewardItems(const TArray<struct FItemReward>& RewardItems);
	bool AddRewardItems(const TArray<struct FItemReward>& RewardItems);

	FInventoryPage& GetPage();

	/*
	*	Request functions for UI
	*/
	bool RequestEquipItem(int32 SlotIndex);
	bool RequestEquipItem(int32 SourceSlotIndex, int32 TargetSlotIndex);
	bool RequestRemoveEquipment(int32 EquipmentIndex, int32 TargetIndex);

	FItemDescription GetItemDescription(int32 SlotIndex);
	int32 RequestItemCount(const FPrimaryAssetId& ItemId);

	FInventoryContentChangedDelegate OnInventoryContentChanged;
	FItemCountChangedDelegate OnItemCountChanged;

	void SetShopMode() { InventoryMode = EInventoryMode::Shop; }
	void SetNormalMode() { InventoryMode = EInventoryMode::Normal; }
	EInventoryMode GetCurrentMode() const { return InventoryMode; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FInventoryPage InventoryPage;

	EInventoryMode InventoryMode;

	UPROPERTY()
	TWeakObjectPtr<class UEquipmentComponent> EquipmentComponentRef;
};
