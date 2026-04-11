// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Item/ItemData.h"
#include "Misc/EnumRange.h"
#include "Inventory.generated.h"

/**
 *   def for inventory page / slot
 */

UENUM(BlueprintType)
enum class EInventoryCategory : uint8
{
	Equipment,
	Consumable,
	Material,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EInventoryCategory, EInventoryCategory::Count); // Register Enum Range using Count

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	bool IsEmpty() const;

	FItemInstance Item;
};

USTRUCT(BlueprintType)
struct FInventoryPage
{
	GENERATED_BODY()

	FInventoryPage();
	FInventoryPage(int32 SlotCountPerPage);

	const FItemInstance& GetItemInstance(int32 SlotIndex);
	bool IsSlotEmpty(int32 SlotIndex) const;
	int32 GetItemAmountById(const FPrimaryAssetId& ItemId) const;

	bool CanAddItem(FItemInstance Item) const;
	bool CanRemoveItem(const FPrimaryAssetId& ItemId, int32 Amount) const;

	bool AddItem(FItemInstance& Item);

	bool RemoveItem(int32 SlotIndex, int32 Amounts = -1);
	bool RemoveItem(const FPrimaryAssetId& ItemId, int32 Amount);
	
	void SwapItems(int32 Index1, int32 Index2);
	bool HasEmptySlot() const;
	int32 GetFirstEmptySlotIndex() const;

	TArray<FInventorySlot> Slots;

	int32 CountMaxSlot;
	int32 CountFilledSlot;
};