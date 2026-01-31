// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward.h"
#include "../Character/Components/Inventory/InventoryComponent.h"
#include "../Character/Components/LevelComponent.h"

bool RewardGrantHelper::TryGrantReward(const FReward& Reward, const FRewardContext& Context)
{
	bool bCanGrantReward = true;

	// Item reward resolve
	{
		bCanGrantReward &= Context.InventoryComponent->CanAddRewardItems(Reward.Items);
	}

	if (!bCanGrantReward)
	{
		return false;
	}

	// Grant Reward
	Context.InventoryComponent->AddRewardItems(Reward.Items);
	Context.LevelComponent->GrantExp(Reward.ExpAmount);

	return true;
}
