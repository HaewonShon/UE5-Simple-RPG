// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSubsystem.h"
#include "../SimpleRPGAssetManager.h"
#include "../Character/SimpleRPGPlayerState.h"
#include "../Enemy/Enemy.h"
#include "QuestManagerComponent.h"

void UQuestManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("QuestData", {}, FStreamableDelegate::CreateUObject(this, &UQuestManagerSubsystem::BuildCache));
}

const UQuestData* UQuestManagerSubsystem::Get(const FPrimaryAssetId& ID) const
{
    UE_LOG(LogQuest, Verbose, TEXT("UQuestManagerSubsystem Quests request ID: %s"), *ID.ToString());
    return QuestCache.FindRef(ID);
}

void UQuestManagerSubsystem::RegisterNPCQuestPair(FPrimaryAssetId NPCId, FPrimaryAssetId QuestId)
{
    UE_LOG(LogQuest, Log, TEXT("Quest Regiestered"));
    NPCQuestMap.Add({ NPCId, QuestId });
}

void UQuestManagerSubsystem::ProcessInteraction(FPrimaryAssetId NPCId, ASimpleRPGPlayerState* PS)
{
    FPrimaryAssetId QuestId = NPCQuestMap[NPCId];
    if (!QuestId.IsValid())
    {
        return;
    }

    UQuestManagerComponent* QuestManagerComponent = PS->GetQuestManagerComponent().Get();
    EQuestStatus QuestStatus = QuestManagerComponent->GetQuestStatus(QuestId);
    if (QuestStatus == EQuestStatus::Completed)
    {
        UE_LOG(LogQuest, Log, TEXT("QuestStatus Completed"));
        return;
    }
    else if (QuestStatus == EQuestStatus::InProgress)
    {
        UE_LOG(LogQuest, Log, TEXT("QuestStatus InProgress"));
        bool bCanComplete = QuestManagerComponent->CanCompleteQuest(QuestId);
        if (bCanComplete)
        {
            QuestManagerComponent->OnCompleteQuest(QuestId);
        }
        else
        {
            // cannot complete quest yet
        }
    }
    else if (QuestStatus == EQuestStatus::NotStarted)
    {
        UE_LOG(LogQuest, Log, TEXT("QuestStatus NotStarted"));
        bool bResult = QuestManagerComponent->RecevieQuest(QuestCache[QuestId]);
    }
    // completed? -> return
    // in progress -> try complete
    // not started -> try start


}

void UQuestManagerSubsystem::GrantQuest(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    if (!PlayerState)
    {
        UE_LOG(LogQuest, Warning, TEXT("GrantQuest Received Player is not valid"));
        return;
    }

    const UQuestData* Quest = Get(QuestId);
    if (!Quest)
    {
        UE_LOG(LogQuest, Warning, TEXT("GrantQuest cannot find quest with Id: %s"), *QuestId.ToString());
        return;
    }

    if (UQuestManagerComponent* QuestComponent = PlayerState->GetComponentByClass<UQuestManagerComponent>())
    {
        bool bResult = QuestComponent->RecevieQuest(Quest);

        // update available quest?
    }
}

void UQuestManagerSubsystem::OnEnenyKilled(const FGameplayTag& EnemyTag, const TArray<FDamageRecord>& DamageHistory)
{
    TSet<ASimpleRPGPlayerState*> PlayersGotEvent;
    for (const FDamageRecord& Record : DamageHistory)
    {
        ASimpleRPGPlayerState* PlayerState = Record.Instigator.Get();
        if (PlayerState && !PlayersGotEvent.Contains(Record.Instigator.Get()))
        {
            PlayerState->NotifyEnemyKilled(EnemyTag);
            PlayersGotEvent.Add(PlayerState);
        }
    }
}

void UQuestManagerSubsystem::OnPlaceVisited(const FGameplayTag& PlaceTag, class ASimpleRPGPlayerState* PlayerState)
{
    if (UQuestManagerComponent* QuestComponent = PlayerState->GetComponentByClass<UQuestManagerComponent>())
    {
        QuestComponent->OnPlaceVisited(PlaceTag);
    }
}

void UQuestManagerSubsystem::BuildCache()
{
    TArray<FPrimaryAssetId> ItemIds;
    USimpleRPGAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("QuestData"), ItemIds);

    for (const FPrimaryAssetId& Id : ItemIds)
    {
        UQuestData* QuestData = USimpleRPGAssetManager::Get().GetPrimaryAssetObject<UQuestData>(Id);
        ensure(QuestData != nullptr);
        QuestCache.Add(Id, QuestData);
    }
    UE_LOG(LogTemp, Log, TEXT("UQuestManagerSubsystem Cache built, Quest asset count: %i"), QuestCache.Num());
}
