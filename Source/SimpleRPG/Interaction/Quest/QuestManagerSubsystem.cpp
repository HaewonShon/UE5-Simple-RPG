// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManagerSubsystem.h"
#include "QuestManagerComponent.h"
#include "QuestIconData.h"
#include "Core/SimpleRPGAssetManager.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Enemy/Enemy.h"
#include "../Core/ActionProvider.h"
#include "../Dialogue/DialogueData.h"
#include "../Dialogue/DialogueSubsystem.h"

// reward context
#include "Player/Components/InventoryComponent.h"
#include "Player/Components/LevelComponent.h"

void UQuestManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("QuestData", {}, FStreamableDelegate::CreateUObject(this, &UQuestManagerSubsystem::BuildCache));
	
	// quest icons in dialogue
	LoadIconData();
}

void UQuestManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	check(InWorld.GetGameInstance());

	DialogueSubsystemRef = InWorld.GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
	check(DialogueSubsystemRef.IsValid());
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
}

TArray<FActionInfo> UQuestManagerSubsystem::CreateQuestActions(FPrimaryAssetId NPCId, ASimpleRPGPlayerState* PlayerState)
{
	TArray<FActionInfo> Actions;
	UQuestManagerComponent* QuestManagerComponent = PlayerState->GetQuestManagerComponent().Get();
	for (FPrimaryAssetId QuestId : NPCQuestMap[NPCId])
	{
		EQuestStatus Status = QuestManagerComponent->GetQuestStatus(QuestId);
		if (!(Status == EQuestStatus::NotStarted || Status == EQuestStatus::InProgress))
		{
			continue;
		}

		const UQuestData* QuestData = Get(QuestId);
		if (!QuestData)
		{
			continue;
		}

		FActionInfo ActionInfo;
		ActionInfo.DisplayName = QuestData->Title;
		ActionInfo.Type = EActionType::QuestSelect;
		ActionInfo.Icon = (Status == EQuestStatus::NotStarted) ? IconDataAsset->QuestAvailableIcon.Get() : IconDataAsset->QuestInProgressIcon.Get();
		ActionInfo.OnActionExecuted.BindUObject(this, &UQuestManagerSubsystem::ResolveQuestSelection, QuestId, PlayerState);
		Actions.Add(ActionInfo);
	}
	return Actions;
}

TArray<FActionInfo> UQuestManagerSubsystem::CreateContextAction(FGameplayTag ActionTag, ASimpleRPGPlayerState* PS)
{
	TArray<FActionInfo> Actions;
	if (ActionTag == FGameplayTag::RequestGameplayTag("Action.Quest.Accept"))
	{
		FActionInfo ActionInfo;
		ActionInfo.DisplayName = FText::FromName(TEXT("Accept"));
		ActionInfo.Type = EActionType::QuestAccept;
		ActionInfo.Icon = nullptr;
		ActionInfo.OnActionExecuted.BindUObject(this, &UQuestManagerSubsystem::ResolveQuestDecision, CurrentSelectedQuestId, PS, true);
		Actions.Add(ActionInfo);
	}
	else if (ActionTag == FGameplayTag::RequestGameplayTag("Action.Quest.Decline"))
	{
		FActionInfo ActionInfo;
		ActionInfo.DisplayName = FText::FromName(TEXT("Decline"));
		ActionInfo.Type = EActionType::QuestDecline;
		ActionInfo.Icon = nullptr;
		ActionInfo.OnActionExecuted.BindUObject(this, &UQuestManagerSubsystem::ResolveQuestDecision, CurrentSelectedQuestId, PS, false);
		Actions.Add(ActionInfo);
	}
	return Actions;
}

void UQuestManagerSubsystem::ResolveQuestSelection(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PlayerState)
{
	UQuestManagerComponent* QuestManagerComponent = PlayerState->GetQuestManagerComponent().Get();
	EQuestStatus QuestStatus = QuestManagerComponent->GetQuestStatus(QuestId);
	const UQuestData* QuestData = Get(QuestId);
	UQuestDialogueData* QuestDialogueData = QuestData->DialogueData;

	if (QuestStatus == EQuestStatus::InProgress)
	{
		UE_LOG(LogQuest, Verbose, TEXT("Resolve Quest Selection %s: In Progress"), *QuestId.ToString());
		bool bCanComplete = QuestManagerComponent->CanClearQuest(QuestId);
		if (bCanComplete)
		{
			QuestManagerComponent->ClearQuest(QuestId);
			int32 EntryNode = QuestData->DialogueData->ContextEntryNodes.FindRef(EQuestDialogueContext::Cleared);
			DialogueSubsystemRef->BeginDialogue(QuestDialogueData, EntryNode);
		}
		else
		{
			// cannot complete quest yet
			int32 EntryNode = QuestData->DialogueData->ContextEntryNodes.FindRef(EQuestDialogueContext::ClearFailed);
			DialogueSubsystemRef->BeginDialogue(QuestDialogueData, EntryNode);
		}
	}
	else if (QuestStatus == EQuestStatus::NotStarted)
	{
		UE_LOG(LogQuest, Verbose, TEXT("Resolve Quest Selection %s: Not started"), *QuestId.ToString());
		CurrentSelectedQuestId = QuestId;

		int32 EntryNode = QuestData->DialogueData->ContextEntryNodes.FindRef(EQuestDialogueContext::Available);
		DialogueSubsystemRef->BeginDialogue(QuestDialogueData, EntryNode);
	}
}

void UQuestManagerSubsystem::ResolveQuestDecision(FPrimaryAssetId QuestId, ASimpleRPGPlayerState* PS, bool bAccepted)
{
	const UQuestData* QuestData = QuestCache[QuestId];
	UQuestDialogueData* QuestDialogueData = QuestData->DialogueData;
	if (bAccepted)
	{
		UQuestManagerComponent* QuestManagerComponent = PS->GetQuestManagerComponent().Get();
		bool bResult = QuestManagerComponent->RecevieQuest(QuestData);

		if (bResult)
		{
			int32 EntryNode = QuestData->DialogueData->ContextEntryNodes.FindRef(EQuestDialogueContext::Accepted);
			DialogueSubsystemRef->BeginDialogue(QuestDialogueData, EntryNode);
			return;
		}
	}

	// decline or failed to receive quest
	if (QuestData->DialogueData)
	{
		int32 EntryNode = QuestData->DialogueData->ContextEntryNodes.FindRef(EQuestDialogueContext::Declined);
		DialogueSubsystemRef->BeginDialogue(QuestDialogueData, EntryNode);
	}
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
			UE_LOG(LogQuest, Verbose, TEXT("TryClearQuest Failed: not able to clear quest"), *QuestId.ToString());
			return false;
		}

		FRewardContext RewardContext;
		RewardContext.InventoryComponent = PlayerState->GetComponentByClass<UInventoryComponent>();
		RewardContext.LevelComponent = PlayerState->GetComponentByClass<ULevelComponent>();
		if (!RewardGrantHelper::TryGrantReward(Get(QuestId)->Reward, RewardContext))
		{
			UE_LOG(LogQuest, Verbose, TEXT("TryClearQuest Failed: not able to grant reward"), *QuestId.ToString());
			return false;
		}

		UE_LOG(LogQuest, Verbose, TEXT("TryClearQuest success: cleared %s"), *QuestId.ToString());
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
	UE_LOG(LogQuest, Log, TEXT("UQuestManagerSubsystem Cache built, Quest asset count: %i"), QuestCache.Num());
}

void UQuestManagerSubsystem::LoadIconData()
{
	UAssetManager& Manager = UAssetManager::Get();

	// 1. 특정 타입의 모든 에셋 리스트를 가져오거나, 특정 ID를 직접 지정
	FPrimaryAssetId TargetId = FPrimaryAssetId("QuestIconData", FName("DA_QuestIcons"));

	// 2. 경로 정보(SoftObjectPath) 가져오기 (메모리 로드 X, 경로만 확보)
	FSoftObjectPath AssetPath = Manager.GetPrimaryAssetPath(TargetId);
	UE_LOG(LogQuest, Log, TEXT("Quest Icons Load request! %s"), *AssetPath.ToString());

	// 3. 필요할 때 비동기 로드 시작
	Manager.LoadPrimaryAsset(TargetId, TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UQuestManagerSubsystem::OnIconDataLoaded));
}

void UQuestManagerSubsystem::OnIconDataLoaded()
{
	FPrimaryAssetId TargetId = FPrimaryAssetId("QuestIconData", FName("DA_QuestIcons"));
	IconDataAsset = UAssetManager::Get().GetPrimaryAssetObject<UQuestIconData>(TargetId);

	if (!IconDataAsset)
	{
		UE_LOG(LogQuest, Warning, TEXT("Loading Quest Icon failed"));
	}
}
