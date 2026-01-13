// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestData.h"
#include "QuestManagerSubsystem.generated.h"

/**
 *		Subsystem for Quest in game
 */
UCLASS()
class SIMPLERPG_API UQuestManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const UQuestData* Get(const FPrimaryAssetId& ID) const;

	void GrantQuest(FPrimaryAssetId QuestId, class ASimpleRPGPlayerState* PlayerState);

	void OnEnenyKilled(const FGameplayTag& Enemy, const TArray<struct FDamageRecord>& DamageHistory);
	void OnPlaceVisited(const FGameplayTag& Place, class ASimpleRPGPlayerState* PlayerState);
	// void OnInteract();

protected:
	void BuildCache();

	UPROPERTY()
	TMap<FPrimaryAssetId, class UQuestData*> QuestCache;
};