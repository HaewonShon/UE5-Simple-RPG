// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameplayTagContainer.h"
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

USTRUCT(BlueprintType)
struct FItemStat
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float AttackPower;

	UPROPERTY(EditDefaultsOnly)
	float Defense;

	UPROPERTY(EditDefaultsOnly)
	float CritChance;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly)
	float HealthRegen;
};

/**
 *	Data Asset for equipment
 */
UCLASS(BlueprintType, meta = (DisplayName = "Equipment Item Data Asset"))
class SIMPLERPG_API UEquipmentItemData : public UItemData
{
	GENERATED_BODY()

public:
	virtual FItemDescription BuildDescriptionData() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EEquipmentType EquipmentType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FItemStat Stat;
};
