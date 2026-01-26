// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "../Character/NPCCharacter.h"
#include "DialogueComponent.h"
#include "DialogueData.h"
#include "../Quest/QuestGiverComponent.h"
#include "../Quest/QuestManagerSubsystem.h"
#include "../Character/SimpleRPGPlayerState.h"

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//for (auto& Pair : QuestStatusIcons)
	//{
	//	//Pair.Value.LoadSynchronous();
	//}

	QuestAvailableIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/Quest_Available"));
	QuestInProgressIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/Quest_InProgress"));
}

void UDialogueSubsystem::BeginDialogue(ANPCCharacter* NPC, ASimpleRPGPlayerState* PS)
{
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dialogue subsys: given PS is not valid"));
		return;
	}

	PlayerStateRef = PS;
	InteractingTargetRef = NPC;


	// set NPC's default dialogue as current one
	if (UDialogueComponent* DialogueComponent = InteractingTargetRef->GetComponentByClass<UDialogueComponent>())
	{
		CurrentDialogueData = DialogueComponent->GetDefaultDialogue();
		CurrentNodeIndex = 0;
	}
	FDialogueInfo DialogueInfo = BuildDialogueWithCurrentNode();

	UQuestManagerSubsystem* QuestSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>();
	check(QuestSubsystem);

	for (const FQuestStatusEntry& Quest : QuestSubsystem->RequestAvailableQuestListForNPC(InteractingTargetRef->GetPrimaryAssetId(), PlayerStateRef.Get()))
	{
		if (const UQuestData* QuestData = QuestSubsystem->Get(Quest.Id))
		{
			DialogueInfo.Responses.Add({ EDialogueResponseType::QuestSelect,
				QuestData->Title,
				QuestData->GetPrimaryAssetId(),
				(Quest.Status == EQuestStatus::NotStarted) ? QuestAvailableIcon : QuestInProgressIcon });
		}
	}

	OnDialogueUpdate.ExecuteIfBound(DialogueInfo);
}

void UDialogueSubsystem::OnDialogueResponses(FDialogueResponse Response)
{
	if (Response.Type == EDialogueResponseType::Continue)
	{
		if (!CurrentDialogueData.Get())
		{
			OnDialogueEnd.Broadcast();
			return;
		}

		int32 NextNode = CurrentDialogueData->DialogueNodes[CurrentNodeIndex].NextNode;
		if (NextNode == -1)
		{
			OnDialogueEnd.Broadcast();
		}
		else
		{
			UpdateDialogueNode(NextNode);
		}
	}
	else if (Response.Type == EDialogueResponseType::Exit)
	{
		OnDialogueEnd.Broadcast();
	}
	else if (Response.Type == EDialogueResponseType::QuestSelect)
	{
		// load quest-related dialogue
		UQuestManagerSubsystem* QuestSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>();
		QuestSubsystem->GrantQuest(Response.Questid, PlayerStateRef.Get());
		OnDialogueEnd.Broadcast();
	}
	else if (Response.Type == EDialogueResponseType::QuestAccept)
	{
		
	}
	else if (Response.Type == EDialogueResponseType::QuestReject)
	{

	}

	// default - quest? exit? 
	// begin new dialogue
	// 
	// quest - accept? decline?
	// accept - questsubsystem에 request
	// decline - 실망이 크다 제군

	// in-quest - cleared?
	// requestclearquest questsubsystem
	// 완료 시 npc는 가나ㅡdquest 목록 update하는게 좋을듯
}

FDialogueInfo UDialogueSubsystem::RequestCurrentDialogueInfo()
{
	return BuildDialogueWithCurrentNode();
}

void UDialogueSubsystem::UpdateDialogueNode(int NextNodeIndex)
{
	CurrentNodeIndex = NextNodeIndex;
	FDialogueInfo DialogueInfo = BuildDialogueWithCurrentNode();

	OnDialogueUpdate.ExecuteIfBound(DialogueInfo);
}

FDialogueInfo UDialogueSubsystem::BuildDialogueWithCurrentNode()
{
	FDialogueInfo DialogueInfo;
	if (CurrentDialogueData.IsValid() && (CurrentNodeIndex != -1 && CurrentNodeIndex < CurrentDialogueData->DialogueNodes.Num()))
	{
		DialogueInfo.NPCName = CurrentDialogueData->NPCName;
		DialogueInfo.DialogueText = CurrentDialogueData->DialogueNodes[CurrentNodeIndex].DialogueText;
	}
	return DialogueInfo;
}
