// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../GameSystem/Reward/Reward.h" // FReward
#include "QuestData.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuest, Log, All);

UENUM(Blueprintable)
enum class EQuestObjectiveType : uint8
{
	Kill,
	Collect,
	Explore,
	Interact,
	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EQuestStatus : uint8
{
	NotStarted,
	InProgress,
	Cleared,
	Count UMETA(Hidden)
};

USTRUCT(Blueprintable)
struct FQuestObjective
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	EQuestObjectiveType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "Type==EQuestObjectiveType::Collect", EditConditionHides))
	FPrimaryAssetId TargetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "Type==EQuestObjectiveType::Explore || Type==EQuestObjectiveType::Kill", EditConditionHides))
	FGameplayTag TargetTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FText TargetDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	int32 RequiredCount;
};

USTRUCT()
struct FRewardItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId ItemId;

	UPROPERTY(EditDefaultsOnly)
	int32 Amount;
};

USTRUCT(Blueprintable)
struct FQuestReward
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FRewardItem> Items;
};

/**
 *	Quest Data Asset
 */
UCLASS(BlueprintType, meta = (DisplayName = "Item Data Asset"))
class SIMPLERPG_API UQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (ReadOnly))
	FPrimaryAssetId AssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	TArray<FQuestObjective> Objectives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	FReward Reward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
	class UQuestDialogueData* DialogueData;
};

USTRUCT()
struct FQuestInstance
{
	GENERATED_BODY()

	FPrimaryAssetId AssetId;

	const UQuestData* QuestData;

	//UObject* ObjectiveStatus;
	TArray<int32> ObjectiveStatus; // UObject for flexibility
};