// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Item/ConsumableItemData.h"
#include "GameplayEffect.h"
#include "InstantConsumableItem.generated.h"

USTRUCT(BlueprintType)
struct FInstantItemEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Health;
};

UCLASS()
class UInstantGE : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UInstantGE();
};

/**
 *  Consumable item data with instant effect - ex) health restore
 */
UCLASS()
class SIMPLERPG_API UInstantConsumableItem : public UConsumableItemData
{
	GENERATED_BODY()
	
public:
	virtual TSubclassOf<class UGameplayEffect> GetGameplayEffectClass() const override;
	virtual bool SetGameplayEffectSpecHandleData(struct FGameplayEffectSpecHandle& SpecHandle) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FInstantItemEffect ItemEffect;
};
