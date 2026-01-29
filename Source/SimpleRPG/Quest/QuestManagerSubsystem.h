// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestData.h"
#include "QuestManagerSubsystem.generated.h"

UENUM()
enum EQuestSelectionResult : uint8
{
	Available,
	Cleared,
	ClearFailed
};

USTRUCT()
struct FQuestStatusEntry
{
	GENERATED_BODY()

	FPrimaryAssetId Id;
	EQuestStatus Status;
};

/**
 *		Subsystem for Quest in game
 */


UCLASS()
class SIMPLERPG_API UQuestManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const UQuestData* Get(const FPrimaryAssetId& ID) const;

	void RegisterNPCQuestPair(FPrimaryAssetId NPCId, FPrimaryAssetId QuestId);
	TArray<FQuestStatusEntry> GetAvailableQuestListForNPC(FPrimaryAssetId NPCId, class ASimpleRPGPlayerState* PlayerState);

	EQuestStatus GetQuestStatus(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);

	void ProcessInteraction(FPrimaryAssetId NPCId, class ASimpleRPGPlayerState* PS);
	EQuestSelectionResult ResolveQuestSelection(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);
	bool TryGrantQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);
	bool CanClearQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);
	bool TryClearQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);

	void OnEnenyKilled(const FGameplayTag& Enemy, const TArray<struct FDamageRecord>& DamageHistory);
	void OnPlaceVisited(const FGameplayTag& Place, class ASimpleRPGPlayerState* PlayerState);

protected:
	void BuildCache();

	TMap<FPrimaryAssetId, TArray<FPrimaryAssetId>> NPCQuestMap; // {npc id, quest ids}

	TMap<FPrimaryAssetId, class UQuestData*> QuestCache;
};