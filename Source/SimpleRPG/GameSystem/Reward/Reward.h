// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.generated.h"

/**
 *	 A reward system which can be used widely
 */

USTRUCT(Blueprintable)
struct FItemReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FPrimaryAssetId ItemId;

	UPROPERTY(EditAnywhere)
	int32 Amount = 1;
};

USTRUCT(Blueprintable)
struct FReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FItemReward> Items;

	UPROPERTY(EditAnywhere)
	int32 ExpAmount;
};

USTRUCT()
struct FRewardContext
{
	GENERATED_BODY()

	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;
	TWeakObjectPtr<class ULevelComponent> LevelComponent;
};

class SIMPLERPG_API RewardGrantHelper
{
public:
	static bool TryGrantReward(const FReward& Reward, const FRewardContext& Context);
};
