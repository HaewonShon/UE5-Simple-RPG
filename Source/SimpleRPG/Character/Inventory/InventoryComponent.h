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

	/*
	*	Inventory Management functions 
	*/
	/* Add Item to inventory */
	bool AddItem(FItemInstance& ItemInstance);

	/* Remove Item From slot. ex) throw out, quest.. */
	void RemoveItem(EInventoryCategory PageCategory, int32 SlotIndex, bool bShouldDropItem);

	void SwapItems(EInventoryCategory PageCategory, int32 Index1, int32 Index2);

	//bool UseItem(UItemData* Item);

	/*
	*	Equipment Management functions
	*/
	void TryEquipItem(int32 SlotIndex, EEquipmentType TargetCategory);

	const FInventoryPage& GetPage(EInventoryCategory PageCategory) const;

	const FInventorySlot& GetEquipmentSlot(EEquipmentType EquipmentType) const;

	FInventoryContentChangedDelegate OnInventoryContentChanged;

protected:
	/* Checker for equipment - category */
	bool CanEquipItem(const FItemInstance& Item, EEquipmentType TargetCategory);
	void EquipItem(const UEquipmentItemData* EquipmentData);
	void UnequipItem(EEquipmentType EquipmentType);

	UPROPERTY()
	TMap<EInventoryCategory, FInventoryPage> InventoryPages;

	UPROPERTY()
	TMap<EEquipmentType, FInventorySlot> EquipmentSlots;
};
