// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "ItemTypes.generated.h"

UENUM()
enum class ERarity : uint8
{
	None,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Relic,
	Count UMETA(Hidden)
};

UENUM()
enum class EStat : uint8
{
	AttackPower,
	Defense,
	CritChance,
	MaxHealth,
	HealthRegen,
	Count UMETA(Hidden)
};

struct FStatLine
{
	FText Name;
	FText Value;
};

struct FItemDetail
{
	FText DetailText;
};

struct FEquipmentDetail
{
	FText TypeText;
	TMap<EStat, FText> Stats;
};

struct FConsumableDetail
{
	TArray<TPair<FText, FText>> Effects;
};

using FItemDetailPayload = TVariant<FItemDetail, FEquipmentDetail, FConsumableDetail>;

/**
 *	structure for delivery item description info
 */
struct FItemDescription
{
	FText Name;
	UTexture2D* Icon;
	FText Price;
	ERarity Rarity;
	FItemDetailPayload Payload;
};

USTRUCT(BlueprintType)
struct FItemStat
{
	GENERATED_BODY()

	FItemStat() : AttackPower(0.f), Defense(0.f), CritChance(0.f), MaxHealth(0.f), HealthRegen(0.f) {}

	FItemStat operator+(const FItemStat& OtherStat) const
	{
		FItemStat Result = *this;
		Result.AttackPower += OtherStat.AttackPower;
		Result.Defense += OtherStat.Defense;
		Result.CritChance += OtherStat.CritChance;
		Result.MaxHealth += OtherStat.MaxHealth;
		Result.HealthRegen += OtherStat.HealthRegen;
		return Result;
	}

	FItemStat& operator+=(const FItemStat& OtherStat)
	{
		AttackPower += OtherStat.AttackPower;
		Defense += OtherStat.Defense;
		CritChance += OtherStat.CritChance;
		MaxHealth += OtherStat.MaxHealth;
		HealthRegen += OtherStat.HealthRegen;
		return *this;
	}

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