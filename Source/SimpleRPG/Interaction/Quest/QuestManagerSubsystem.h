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
	FText Title;
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
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	const UQuestData* Get(const FPrimaryAssetId& ID) const;

	void RegisterNPCQuestPair(FPrimaryAssetId NPCId, FPrimaryAssetId QuestId);
	TArray<struct FActionInfo> CreateQuestActions(FPrimaryAssetId NPCId, class ASimpleRPGPlayerState* PlayerState);
	struct FActionInfo CreateContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS);

	EQuestStatus GetQuestStatus(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);

	void ResolveQuestSelection(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PS);
	void ResolveQuestDecision(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PS, bool bAccepted);

	bool TryGrantQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);
	bool CanClearQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);
	bool TryClearQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);

	void OnEnenyKilled(const FGameplayTag& Enemy, const TArray<struct FDamageRecord>& DamageHistory);
	void OnPlaceVisited(const FGameplayTag& Place, class ASimpleRPGPlayerState* PlayerState);

protected:
	void BuildCache();

	void LoadIconData();
	void OnIconDataLoaded();

	TWeakObjectPtr<class UDialogueSubsystem> DialogueSubsystemRef;

	TMap<FPrimaryAssetId, TArray<FPrimaryAssetId>> NPCQuestMap; // {npc id, quest ids}

	TMap<FPrimaryAssetId, class UQuestData*> QuestCache;

	// icons for dialogue
	UPROPERTY()
	TObjectPtr<class UQuestIconData> IconDataAsset;

	FPrimaryAssetId CurrentSelectedQuestId;
};