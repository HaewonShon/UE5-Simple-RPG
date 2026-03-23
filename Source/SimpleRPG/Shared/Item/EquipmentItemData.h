// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameplayTagContainer.h"
#include "EquipmentItemData.generated.h"

UENUM()
enum class EEquipmentType : uint8
{
	Weapon = 0,
	Helmet = 1,
	Chest = 2,
	Pants = 3,
	Boots = 4,
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
	virtual bool CanExecute(AActor* Executer, const FSlotAddress& Address) const override;
	virtual void Execute(AActor* Executer, const FSlotAddress& Address) override;

	virtual bool CanEnhance() const override { return true; }
	virtual FItemStat GetBaseStat() const override { return BaseStat; }

	virtual FItemDescription BuildDescriptionData() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EEquipmentType EquipmentType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FItemStat BaseStat;
};
