// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent.h"
#include "../Character/SimpleRPGPlayerController.h"

void UDialogueComponent::Interact(ASimpleRPGPlayerController* Controller)
{
	if (!DefaultDialogue)
	{
		return;
	}

	if (Controller)
	{
		UE_LOG(LogTemp, Log, TEXT("Dialogue requested"));
		Controller->OnDialogueRequested.Broadcast(GetOwner(), DefaultDialogue);
	}
}