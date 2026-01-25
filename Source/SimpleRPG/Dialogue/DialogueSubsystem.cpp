// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "../Character/NPCCharacter.h"
#include "DialogueComponent.h"
#include "DialogueData.h"
#include "../Quest/QuestGiverComponent.h"
#include "../Quest/QuestManagerSubsystem.h"

void UDialogueSubsystem::BeginDefaultDialogue(FPrimaryAssetId NPCId, class UDialogueData* DefaultDialogue)
{
	// set NPC's default dialogue as current one
	CurrentDialogueData = DefaultDialogue;
	CurrentNodeIndex = 0;

	FDialogueInfo DialogueInfo = BuildDialogueWithCurrentNode();

	UQuestManagerSubsystem* QuestSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>();
	check(QuestSubsystem);

	//QuestSubsystem->GetAvailableQuestByNPCId(NPCId);
	//if (UQuestGiverComponent* QuestGiverComponent = NPC->GetComponentByClass<UQuestGiverComponent>())
	{
		// quest system -> check if the quest is available
		// DialogueInfo.Responses.Add --- questselecthandle
	}

	OnDialogueUpdate.ExecuteIfBound(DialogueInfo);
}

void UDialogueSubsystem::OnDialogueResponses(FDialogueResponse Response)
{
	if (Response.Type == EDialogueResponseType::Continue)
	{
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
	DialogueInfo.NPCName = CurrentDialogueData->NPCName;
	DialogueInfo.DialogueText = CurrentDialogueData->DialogueNodes[CurrentNodeIndex].DialogueText;
	return DialogueInfo;
}
