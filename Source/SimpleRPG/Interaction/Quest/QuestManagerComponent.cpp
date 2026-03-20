// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerComponent.h"
#include "QuestData.h"
#include "Player/Components/InventoryComponent.h"

// Sets default values for this component's properties
UQuestManagerComponent::UQuestManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestManagerComponent::SetInventoryComponentRef(UInventoryComponent* InventoryComponent)
{
	InventoryComponentRef = InventoryComponent;
	if (InventoryComponentRef.IsValid())
	{
		InventoryComponentRef->OnItemCountChanged.AddUObject(this, &UQuestManagerComponent::OnItemCountChanged);
	}
	else
	{
		UE_LOG(LogQuest, Warning, TEXT("Failed to set inventory component ref"));
	}
}

EQuestStatus UQuestManagerComponent::GetQuestStatus(const FPrimaryAssetId& QuestId) const
{
	if (ClearedQuestSet.Contains(QuestId))
	{
		return EQuestStatus::Cleared;
	}
	if (QuestInProgress.Contains(QuestId))
	{
		return EQuestStatus::InProgress;
	}

	return EQuestStatus::NotStarted;
}

bool UQuestManagerComponent::CanClearQuest(const FPrimaryAssetId& QuestId) const
{
	FQuestInstance Instance = QuestInProgress[QuestId];
	if (!Instance.QuestData)
	{
		return false;
	}

	const TArray<FQuestObjective>& Objectives = Instance.QuestData->Objectives;
	for (int32 i = 0; i < Objectives.Num(); ++i)
	{
		if (Instance.ObjectiveStatus[i] < Objectives[i].RequiredCount)
		{
			return false;
		}
	}
	return true;
}

bool UQuestManagerComponent::RecevieQuest(const UQuestData* Quest)
{
	if (QuestInProgress.Num() >= MAX_QUEST_COUNT)
	{
		return false;
	}

	if (QuestInProgress.Find(Quest->AssetId))
	{
		return false;
	}

	FQuestInstance QuestInstance;
	QuestInstance.QuestData = Quest;
	QuestInstance.AssetId = Quest->GetPrimaryAssetId();
	QuestInstance.ObjectiveStatus.SetNum(Quest->Objectives.Num());

	QuestInProgress.Add({ QuestInstance.AssetId, QuestInstance });
	UE_LOG(LogQuest, Log, TEXT("Quest received: %s"), *QuestInstance.AssetId.ToString());

	OnQuestAccepted.ExecuteIfBound(Quest);

	return true;
}

void UQuestManagerComponent::ClearQuest(const FPrimaryAssetId& CompletedQuestId)
{
	FQuestInstance Instance = QuestInProgress[CompletedQuestId];
	if (!Instance.QuestData)
	{
		return;
	}

	UE_LOG(LogQuest, Log, TEXT("Quest completed: %s"), *CompletedQuestId.ToString());
	
	QuestInProgress.Remove(CompletedQuestId);
	ClearedQuestSet.Add(CompletedQuestId);

	OnQuestCompleted.ExecuteIfBound(CompletedQuestId);
}

void UQuestManagerComponent::OnEnemyKilled(const FGameplayTag& EnemyTag)
{
	for (auto& Pair : QuestInProgress)
	{
		FQuestInstance& Instance = Pair.Value;
		const UQuestData* Quest = Instance.QuestData;
		for(int32 i = 0; i < Quest->Objectives.Num(); ++i)
		{
			const FQuestObjective& Objective = Quest->Objectives[i];
			if (Instance.ObjectiveStatus[i] >= Objective.RequiredCount)
			{
				continue;
			}

			if (Objective.Type == EQuestObjectiveType::Kill && Objective.TargetTag == EnemyTag)
			{
				OnQuestProgressChanged.Broadcast(Instance.AssetId, i, ++Instance.ObjectiveStatus[i]);
			}
		}
	}
}

void UQuestManagerComponent::OnPlaceVisited(const FGameplayTag& PlaceTag)
{
	for (auto& Pair : QuestInProgress)
	{
		FQuestInstance& Instance = Pair.Value;
		const UQuestData* Quest = Instance.QuestData;
		for (int32 i = 0; i < Quest->Objectives.Num(); ++i)
		{
			const FQuestObjective& Objective = Quest->Objectives[i];
			if (Instance.ObjectiveStatus[i] >= Objective.RequiredCount)
			{
				continue;
			}

			if (Objective.Type == EQuestObjectiveType::Explore && Objective.TargetTag == PlaceTag)
			{
				OnQuestProgressChanged.Broadcast(Instance.AssetId, i, ++Instance.ObjectiveStatus[i]);
			}
		}
	}
}

void UQuestManagerComponent::OnItemCountChanged(const FPrimaryAssetId& ItemId)
{
	for (auto& Pair : QuestInProgress)
	{
		FQuestInstance& Instance = Pair.Value;
		const UQuestData* Quest = Instance.QuestData;
		for (int32 i = 0; i < Quest->Objectives.Num(); ++i)
		{
			const FQuestObjective& Objective = Quest->Objectives[i];
			if (Objective.Type == EQuestObjectiveType::Collect && Objective.TargetId == ItemId)
			{
				int32 ItemCount = InventoryComponentRef->RequestItemCount(ItemId);
				Instance.ObjectiveStatus[i] = ItemCount;
				OnQuestProgressChanged.Broadcast(Instance.AssetId, i, Instance.ObjectiveStatus[i]);
			}
		}
	}
}
