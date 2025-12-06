// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Item/ItemData.h"
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

	bool AddItem(FItemInstance& Item);
	bool HasEmptySlot() const;

	EInventoryCategory Category;

	TArray<FInventorySlot> Slots;

	int32 CountMaxSlot;
	int32 CountFilledSlot;
};