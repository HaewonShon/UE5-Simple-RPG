// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent.h"
#include "../Character/SimpleRPGPlayerController.h"
#include "../Character/SimpleRPGPlayerState.h"
#include "DialogueSubsystem.h"


void UDialogueComponent::BeginDialogue(ASimpleRPGPlayerController* Controller)
{
	if (!DefaultDialogue)
	{
		return;
	}

	if (UDialogueSubsystem* Subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		//ASimpleRPGPlayerState* PS = Controller->GetPlayerState<ASimpleRPGPlayerState>();
		//Subsystem->BeginDialogue(GetOwner()->GetPrimaryAssetId(), PS, DefaultDialogue);
	}
}

UDialogueData* UDialogueComponent::GetDefaultDialogue() const
{
	return DefaultDialogue;
}

UDialogueData* UDialogueComponent::GetQuestDialogue(FPrimaryAssetId QuestId) const
{
	if (QuestDialogues.Find(QuestId))
	{
		return QuestDialogues[QuestId];
	}
	return nullptr;
}

