// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward.h"
#include "../Character/Inventory/InventoryComponent.h"

bool RewardGrantHelper::TryGrantReward(const FReward& Reward, const FRewardContext& Context)
{
	bool bCanGrantReward = true;

	// Item reward resolve
	{
		bCanGrantReward &= Context.Inventory->CanAddRewardItems(Reward.Items);
	}

	if (!bCanGrantReward)
	{
		return false;
	}

	// Grant Reward
	Context.Inventory->AddRewardItems(Reward.Items);

	return true;
}
