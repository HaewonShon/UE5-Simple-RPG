// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueComponent.h"
#include "Player/SimpleRPGPlayerController.h"
#include "Player/SimpleRPGPlayerState.h"
#include "DialogueSubsystem.h"

UDialogueData* UDialogueComponent::GetDefaultDialogue() const
{
	return DefaultDialogue;
}

