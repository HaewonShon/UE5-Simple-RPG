// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "EquipmentItemData.generated.h"

UENUM()
enum class EEquipmentType : uint8
{
	Weapon,
	Helmet,
	Chest,
	Pants,
	Boots,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EEquipmentType, EEquipmentType::Count);

/**
 *	Data Asset for equipment
 */
UCLASS(BlueprintType, meta = (DisplayName = "Equipment Item Data Asset"))
class SIMPLERPG_API UEquipmentItemData : public UItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EEquipmentType EquipmentCategory;

protected:
};
