// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestData.h"
#include "QuestManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnQuestAccepted, const class UQuestData*);
DECLARE_DELEGATE_ThreeParams(FOnQuestProgressChanged, FPrimaryAssetId, int32, int32); // QuestId, Objective Index, Progress

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestManagerComponent();
	
	// Called by Quest Subsystem, adds a quest to the component
	bool RecevieQuest(const class UQuestData* Quest);

	void OnCompleteQuest(FPrimaryAssetId CompletedQuestId);

	void OnEnemyKilled(const FGameplayTag& EnemyTag);

	FOnQuestAccepted OnQuestAccepted;
	FOnQuestProgressChanged OnQuestProgressChanged;

protected:
	TMap<FPrimaryAssetId, FQuestInstance> QuestInProgress;

	/*UFUNCTION()
	void OnMonsterKilled();

	UFUNCTION()
	void OnSpaceVisited();

	UFUNCTION()
	void OnItemEarned();*/

	static const int32 MAX_QUEST_COUNT = 5;
};
