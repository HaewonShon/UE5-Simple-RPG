// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	INVENTORY_CATEGORY_COUNT
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	TObjectPtr<class UItemData> Item;
};

USTRUCT(BlueprintType)
struct FInventoryPage
{
	GENERATED_BODY()

	FInventoryPage();

	EInventoryCategory Category;

	TMap <TPair<int, int>, FInventorySlot > Slots;
};