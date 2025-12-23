// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "EquipmentItemData.generated.h"

UENUM()
enum class EEquipmentCategory : uint8
{
	Weapon,
	Helemt,
	Chest,
	Pants,
	Boots,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EEquipmentCategory, EEquipmentCategory::Count);

/**
 *	Data Asset for equipment
 */
UCLASS(BlueprintType, meta = (DisplayName = "Item Data Asset"))
class SIMPLERPG_API UEquipmentItemData : public UItemData
{
	GENERATED_BODY()

public:
	EEquipmentCategory EquipmentCategory;

protected:
};
