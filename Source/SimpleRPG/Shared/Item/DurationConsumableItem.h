// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Item/ConsumableItemData.h"
#include "GameplayEffect.h"
#include "DurationConsumableItem.generated.h"

USTRUCT(BlueprintType)
struct FDurationItemEffect
{
	GENERATED_BODY()

	FDurationItemEffect() : AttackPower(0.f), Defense(0.f), CritChance(0.f), MaxHealth(0.f), HealthRegen(0.f) {}

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

UCLASS()
class UDurationGE : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UDurationGE();
};

/**
 *  Consumable item data with duration - ex) attack buff
 */
UCLASS()
class SIMPLERPG_API UDurationConsumableItem : public UConsumableItemData
{
	GENERATED_BODY()
public:
	virtual TSubclassOf<class UGameplayEffect> GetGameplayEffectClass() const override;
	virtual bool SetGameplayEffectSpecHandleData(struct FGameplayEffectSpecHandle& SpecHandle) const override;

protected:

	UPROPERTY(EditDefaultsOnly)
	float Duration;

	UPROPERTY(EditDefaultsOnly)
	FDurationItemEffect ItemEffect;
};
