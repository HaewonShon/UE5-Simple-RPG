// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "DialogueComponent.h"
#include "DialogueData.h"
#include "World/NPCCharacter.h"
#include "Player/SimpleRPGPlayerState.h"
#include "../Quest/QuestGiverComponent.h"
#include "../Quest/QuestManagerSubsystem.h"

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDialogueSubsystem::BeginDefaultDialogue(ANPCCharacter* NPC, ASimpleRPGPlayerState* PS)
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

	/* add actions using IActionProvider*/
	for (UActorComponent* Component : NPC->GetComponentsByInterface(UActionProvider::StaticClass()))
	{
		TArray<FActionInfo> Actions = Cast<IActionProvider>(Component)->CreateAvailableActions(PS);
		DialogueInfo.Actions.Append(Actions);
	}

	OnDialogueUpdate.ExecuteIfBound(DialogueInfo);
}

void UDialogueSubsystem::BeginDialogue(UDialogueData* Dialogue, int32 DialogueBeginNode)
{
	CurrentDialogueData = Dialogue;
	UpdateDialogueNode(DialogueBeginNode);
}

void UDialogueSubsystem::SetContextOwner(UActorComponent* Owner)
{
	ContextOwnerRef = Owner;
}

void UDialogueSubsystem::SetNextPage()
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
	if (CurrentDialogueData.IsValid() && 
		(CurrentNodeIndex != -1 && CurrentNodeIndex < CurrentDialogueData->DialogueNodes.Num()))
	{
		const FDialogueNode& Node = CurrentDialogueData->DialogueNodes[CurrentNodeIndex];
		DialogueInfo.NPCName = CurrentDialogueData->NPCName;
		DialogueInfo.DialogueText = Node.DialogueText;
		// request custom action to owner if exist
		if (ContextOwnerRef.IsValid())
		{
			for(FGameplayTag CustomAction : Node.CustomActions)
			{
				DialogueInfo.Actions.Add(Cast<IActionProvider>(ContextOwnerRef)->CreateContextAction(CustomAction, PlayerStateRef.Get()));
			}
		}
	}
	return DialogueInfo;
}