// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Item/ItemData.h"
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
	FInventoryPage(EInventoryCategory PageCategory, int32 SlotCountPerPage);

	const FItemInstance& GetItemInstance(int32 SlotIndex);
	bool IsSlotEmpty(int32 SlotIndex) const;

	bool AddItem(FItemInstance& Item);
	void RemoveItem(int32 SlotIndex);
	void SwapItems(int32 Index1, int32 Index2);
	bool HasEmptySlot() const;
	int32 GetFirstEmptySlotIndex() const;

	EInventoryCategory Category;
	TArray<FInventorySlot> Slots;

	int32 CountMaxSlot;
	int32 CountFilledSlot;
};