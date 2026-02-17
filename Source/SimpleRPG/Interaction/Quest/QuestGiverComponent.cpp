// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestGiverComponent.h"
#include "QuestManagerSubsystem.h"
#include "World/NPCCharacter.h"

// Sets default values for this component's properties
UQuestGiverComponent::UQuestGiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<FActionInfo> UQuestGiverComponent::CreateAvailableActions(ASimpleRPGPlayerState* PS)
{
	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		TArray<FActionInfo> Actions = Subsystem->CreateQuestActions(Cast<ANPCCharacter>(GetOwner())->GetPrimaryAssetId(), PS);
		for (FActionInfo& Action : Actions)
		{
			Action.ContextOwner = this;
		}
		return Actions;
	}
	return TArray<FActionInfo>();
}

FActionInfo UQuestGiverComponent::CreateContextAction(FGameplayTag ActionTag, ASimpleRPGPlayerState* PS)
{
	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		FActionInfo Action = Subsystem->CreateContextAction(ActionTag, PS);
		Action.ContextOwner = this;
		return Action;
	}
	return FActionInfo();
}

// Called when the game starts
void UQuestGiverComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		Subsystem->RegisterNPCQuestPair(Cast<ANPCCharacter>(GetOwner())->GetPrimaryAssetId(), QuestList[0]);
	}
}