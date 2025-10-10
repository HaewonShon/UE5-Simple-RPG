// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestData.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
	Kill,
	Collect,
	Explore,
	Interact,
	QUEST_OBJECTIVE_TYPE_MAX
};

UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
	Exp,
	Item,
	QUEST_REWARD_TYPE_MAX
};

USTRUCT(BlueprintType)
struct FQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	EQuestObjectiveType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FName TargetID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	int32 RequiredCount;
};

USTRUCT(BlueprintType)
struct FQuestReward
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	EQuestRewardType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FName RewardID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	int32 RewardAmount;
};

UCLASS()
class SIMPLERPG_API UQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestObjective> Objectives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestReward> Rewards;
};