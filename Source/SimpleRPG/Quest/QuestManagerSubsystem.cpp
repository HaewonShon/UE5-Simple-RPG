// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSubsystem.h"
#include "../SimpleRPGAssetManager.h"
#include "../Character/SimpleRPGPlayerState.h"
#include "../Enemy/Enemy.h"
#include "QuestManagerComponent.h"
#include "../Item/ItemDatabaseSubsystem.h"
#include "../Character/Inventory/InventoryComponent.h"

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
    if (!NPCQuestMap.Find(NPCId))
    {
        NPCQuestMap.Add({ NPCId });
    }

    NPCQuestMap[NPCId].Add(QuestId);
    UE_LOG(LogQuest, Log, TEXT("Quest Regiestered"));
}

TArray<FQuestStatusEntry> UQuestManagerSubsystem::GetAvailableQuestListForNPC(FPrimaryAssetId NPCId, ASimpleRPGPlayerState* PlayerState)
{
    TArray<FQuestStatusEntry> QuestList;

    UQuestManagerComponent* QuestManagerComponent = PlayerState->GetQuestManagerComponent().Get();
    for (FPrimaryAssetId QuestId : NPCQuestMap[NPCId])
    {
        EQuestStatus Status = QuestManagerComponent->GetQuestStatus(QuestId);
        if (Status == EQuestStatus::NotStarted || Status == EQuestStatus::InProgress)
        {
            QuestList.Add({QuestId, Status});
        }
    }
    return QuestList;
}

void UQuestManagerSubsystem::ProcessInteraction(FPrimaryAssetId NPCId, ASimpleRPGPlayerState* PlayerState)
{
    FPrimaryAssetId QuestId = NPCQuestMap[NPCId][0];
    if (!QuestId.IsValid())
    {
        return;
    }

    UQuestManagerComponent* QuestManagerComponent = PlayerState->GetQuestManagerComponent().Get();
    EQuestStatus QuestStatus = QuestManagerComponent->GetQuestStatus(QuestId);
    if (QuestStatus == EQuestStatus::Cleared)
    {
        UE_LOG(LogQuest, Log, TEXT("QuestStatus Completed"));
        return;
    }
    else if (QuestStatus == EQuestStatus::InProgress)
    {
        UE_LOG(LogQuest, Log, TEXT("QuestStatus InProgress"));
        bool bCanComplete = QuestManagerComponent->CanClearQuest(QuestId);
        if (bCanComplete)
        {
            QuestManagerComponent->ClearQuest(QuestId);
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

EQuestSelectionResult UQuestManagerSubsystem::ResolveQuestSelection(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    EQuestStatus QuestStatus = GetQuestStatus(QuestId, PlayerState);
    if (QuestStatus == EQuestStatus::NotStarted)
    {
        return EQuestSelectionResult::Available;
    }
    else if (QuestStatus == EQuestStatus::InProgress)
    {
        bool bIsQuestCleared = TryClearQuest(QuestId, PlayerState);
        if(bIsQuestCleared)
        {
            return EQuestSelectionResult::Cleared;
        }
        else
        {
            return EQuestSelectionResult::ClearFailed;
        }
    };
    return EQuestSelectionResult::Available;
}

EQuestStatus UQuestManagerSubsystem::GetQuestStatus(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    if (UQuestManagerComponent* Component = PlayerState->GetQuestManagerComponent().Get())
    {
        return Component->GetQuestStatus(QuestId);
    }
    return EQuestStatus::Count;
}

bool UQuestManagerSubsystem::TryGrantQuest(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    if (!PlayerState)
    {
        UE_LOG(LogQuest, Warning, TEXT("GrantQuest Received Player is not valid"));
        return false;
    }

    const UQuestData* Quest = Get(QuestId);
    if (!Quest)
    {
        UE_LOG(LogQuest, Warning, TEXT("GrantQuest cannot find quest with Id: %s"), *QuestId.ToString());
        return false;
    }

    if (UQuestManagerComponent* QuestComponent = PlayerState->GetComponentByClass<UQuestManagerComponent>())
    {
        bool bResult = QuestComponent->RecevieQuest(Quest);
        return bResult;

        // update available quest?
    }
    return false;
}

bool UQuestManagerSubsystem::CanClearQuest(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    if (UQuestManagerComponent* QuestComponent = PlayerState->GetComponentByClass<UQuestManagerComponent>())
    {
        return QuestComponent->CanClearQuest(QuestId);
    }
    return false;
}


bool UQuestManagerSubsystem::TryClearQuest(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
    if (UQuestManagerComponent* QuestComponent = PlayerState->GetComponentByClass<UQuestManagerComponent>())
    {
        if (!QuestComponent->CanClearQuest(QuestId))
        {
            UE_LOG(LogQuest, Warning, TEXT("TryClearQuest Failed: not able to clear quest"), *QuestId.ToString());
            return false;
        }

        FRewardContext RewardContext;
        RewardContext.Inventory = PlayerState->GetComponentByClass<UInventoryComponent>();
        if (!RewardGrantHelper::TryGrantReward(Get(QuestId)->Reward, RewardContext))
        {
            UE_LOG(LogQuest, Warning, TEXT("TryClearQuest Failed: not able to grant reward"), *QuestId.ToString());
            return false;
        }

        UE_LOG(LogQuest, Warning, TEXT("TryClearQuest success: cleared %s"), *QuestId.ToString());
        QuestComponent->ClearQuest(QuestId);
        return true;
    }

    return false;
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
{//
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