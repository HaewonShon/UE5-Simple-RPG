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

	for (const FQuestStatusEntry& Quest : QuestSubsystem->GetAvailableQuestListForNPC(InteractingTargetRef->GetPrimaryAssetId(), PlayerStateRef.Get()))
	{
		if (const UQuestData* QuestData = QuestSubsystem->Get(Quest.Id))
		{
			DialogueInfo.Responses.Add({ EDialogueResponseType::QuestSelect,
				QuestData->GetPrimaryAssetId(),
				QuestData->Title,
				(Quest.Status == EQuestStatus::NotStarted) ? QuestAvailableIcon : QuestInProgressIcon });
		}
	}

	OnDialogueUpdate.ExecuteIfBound(DialogueInfo);
}

void UDialogueSubsystem::OnDialogueResponse(FDialogueResponse Response)
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
		CurrentQuestId = Response.QuestId;
		HandleQuestSelectResponse(Response);
	}
	else if (Response.Type == EDialogueResponseType::QuestAccept)
	{
		UQuestManagerSubsystem* QuestSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>();
		bool bResult = QuestSubsystem->TryGrantQuest(Response.QuestId, PlayerStateRef.Get());

		UQuestDialogueData* QuestDialogue = Cast<UQuestDialogueData>(CurrentDialogueData);
		int32 Node = QuestDialogue->ContextEntryNodes[EQuestDialogueContext::Accepted];
		UpdateDialogueNode(Node);
	}
	else if (Response.Type == EDialogueResponseType::QuestDecline)
	{
		UQuestDialogueData* QuestDialogue = Cast<UQuestDialogueData>(CurrentDialogueData);
		int32 Node = QuestDialogue->ContextEntryNodes[EQuestDialogueContext::Declined];
		UpdateDialogueNode(Node);
	}
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

void UDialogueSubsystem::HandleQuestSelectResponse(const FDialogueResponse& Response)
{
	UQuestManagerSubsystem* QuestSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>();
	const UQuestData* QuestData = QuestSubsystem->Get(Response.QuestId);

	CurrentDialogueData = QuestData->DialogueData;
	UQuestDialogueData* QuestDialogue = Cast<UQuestDialogueData>(CurrentDialogueData);

	EQuestSelectionResult Result = QuestSubsystem->ResolveQuestSelection(Response.QuestId, PlayerStateRef.Get());

	int NextNode = QuestDialogue->ContextEntryNodes[ConvertQuestSelectionResultToContext(Result)];
	UpdateDialogueNode(NextNode);
}

FDialogueInfo UDialogueSubsystem::BuildDialogueWithCurrentNode()
{
	FDialogueInfo DialogueInfo;
	if (CurrentDialogueData.IsValid() && (CurrentNodeIndex != -1 && CurrentNodeIndex < CurrentDialogueData->DialogueNodes.Num()))
	{
		const FDialogueNode& Node = CurrentDialogueData->DialogueNodes[CurrentNodeIndex];
		DialogueInfo.NPCName = CurrentDialogueData->NPCName;
		DialogueInfo.DialogueText = Node.DialogueText;

		for (EDialogueResponseType ResponseType : Node.Responses)
		{
			DialogueInfo.Responses.Add({ ResponseType, CurrentQuestId });
		}
	}
	return DialogueInfo;
}

EQuestDialogueContext UDialogueSubsystem::ConvertQuestSelectionResultToContext(EQuestSelectionResult SelectionResult)
{
	switch (SelectionResult)
	{
	case EQuestSelectionResult::Available:
		return EQuestDialogueContext::Available;
	case EQuestSelectionResult::Cleared:
		return EQuestDialogueContext::Cleared;
	case EQuestSelectionResult::ClearFailed:
		return EQuestDialogueContext::ClearFailed;
	}
	return EQuestDialogueContext();
}
