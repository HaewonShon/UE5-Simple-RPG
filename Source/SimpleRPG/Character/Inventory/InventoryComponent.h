// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"
#include "../../Item/ItemData.h"
#include "../../Item/EquipmentItemData.h"
#include "InventoryComponent.generated.h"

// Define Log Inventory for Inventory-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All)

DECLARE_DELEGATE_OneParam(FInventoryContentChangedDelegate, EInventoryCategory)

UCLASS(ClassGroup = (SimpleRPG), meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;

	/* Add Item to inventory */
	bool AddItem(FItemInstance& ItemInstance);

	/* Remove Item From slot. ex) throw out, quest.. */
	void RemoveItem(EInventoryCategory PageCategory, int32 SlotIndex, bool bShouldDropItem);

	//void RequestEquipItem(FInventoryPage Page, int32 SlotIndex, )
	//bool UnequipItem(UItemData* Item);
	//bool UseItem(UItemData* Item);

	void SwapItems(EInventoryCategory PageCategory, int32 Index1, int32 Index2);

	const FInventoryPage& GetPage(EInventoryCategory PageCategory) const;

	FInventoryContentChangedDelegate OnInventoryContentChanged;

protected:
	bool TryEquipItem(const FItemInstance& Item, EEquipmentCategory TargetCategory);

	UPROPERTY()
	TMap<EInventoryCategory, FInventoryPage> InventoryPages;

	UPROPERTY()
	TMap<EEquipmentCategory, FInventorySlot> EquiupmentSlotMap;
};
