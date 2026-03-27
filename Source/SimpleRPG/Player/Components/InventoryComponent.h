// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"
#include "Shared/Item/ItemData.h"
#include "InventoryComponent.generated.h"

// Define Log Inventory for Inventory-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All)

DECLARE_MULTICAST_DELEGATE(FInventoryContentChangedDelegate)
DECLARE_MULTICAST_DELEGATE_OneParam(FItemCountChangedDelegate, const FPrimaryAssetId&)

UENUM()
enum class EInventoryMode : uint8
{
	Normal,
	Shop,
	Enhancement,
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
	bool RemoveItem(int32 SlotIndex, bool bShouldDropItem);
	bool RemoveItem(int32 SlotIndex, int32 Count);

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

	bool RequestUseItem(int32 SlotIndex);
	bool RequestSellItem(int32 SlotIndex);
	bool RequestPurchaseItem(int32 ShopSlotIndex);

	bool RequestRegisterEnhanceTarget(int32 SlotIndex);

	FItemDescription GetItemDescription(int32 SlotIndex);
	int32 RequestItemCount(const FPrimaryAssetId& ItemId);

	FInventoryContentChangedDelegate OnInventoryContentChanged;
	FItemCountChangedDelegate OnItemCountChanged;

	void SetShopMode(class UShopComponent* ShopCmopRef);
	void SetEnhanceMode(); 
	void SetNormalMode();
	EInventoryMode GetCurrentMode() const { return InventoryMode; }
protected:
	virtual void BeginPlay() override;
	void TryUseItem(int32 SlotIndex);

	UPROPERTY()
	FInventoryPage InventoryPage;

	UPROPERTY()
	TWeakObjectPtr<class UEquipmentComponent> EquipmentComponentRef;

	UPROPERTY()
	TWeakObjectPtr<class UShopComponent> InteractingShopComponentRef;

	EInventoryMode InventoryMode;
};
