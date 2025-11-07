// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Item/ItemData.h"
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

	FItemInstance Item;
};

USTRUCT(BlueprintType)
struct FInventoryPage
{
	GENERATED_BODY()

	FInventoryPage();

	FInventoryPage(EInventoryCategory PageCategory);

	bool AddItem(FItemInstance Item);

	EInventoryCategory Category;

	TArray<FInventorySlot> Slots;
	//TMap <TPair<int32, int32>, FInventorySlot > Slots;
};