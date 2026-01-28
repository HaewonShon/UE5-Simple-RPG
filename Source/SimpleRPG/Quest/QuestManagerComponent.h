// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestData.h"
#include "QuestManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnQuestAccepted, const class UQuestData*);
DECLARE_DELEGATE_OneParam(FOnQuestCompleted, FPrimaryAssetId);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnQuestProgressChanged, FPrimaryAssetId, int32, int32); // QuestId, Objective Index, Progress

/*
*	Quest Component for player state, manage quest status
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestManagerComponent();
	void SetInventoryComponentRef(class UInventoryComponent* InventoryComponent);

	EQuestStatus GetQuestStatus(const FPrimaryAssetId& QuestId) const;
	bool CanClearQuest(const FPrimaryAssetId& QuestId) const;
	
	// Called by Quest Subsystem, adds a quest to the component
	bool RecevieQuest(const class UQuestData* Quest);
	void ClearQuest(const FPrimaryAssetId& CompletedQuestId);

	void OnEnemyKilled(const FGameplayTag& EnemyTag);
	void OnPlaceVisited(const FGameplayTag& PlaceTag);

	FOnQuestAccepted OnQuestAccepted;
	FOnQuestProgressChanged OnQuestProgressChanged;
	FOnQuestCompleted OnQuestCompleted;

protected:
	void OnItemCountChanged(const FPrimaryAssetId& ItemId);

	TMap<FPrimaryAssetId, FQuestInstance> QuestInProgress;
	static const int32 MAX_QUEST_COUNT = 5;

	TSet<FPrimaryAssetId> ClearedQuestSet;

	TWeakObjectPtr<class UInventoryComponent> InventoryComponentRef;
};
