// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestGiverComponent.h"
#include "QuestManagerSubsystem.h"
#include "../Character/NPCCharacter.h"

// Sets default values for this component's properties
UQuestGiverComponent::UQuestGiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UQuestGiverComponent::Oninteraction(FPrimaryAssetId NPCId, ASimpleRPGPlayerState* PS)
{
	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		//Subsystem->GrantQuest(QuestList[0], PS);
		//Subsystem->ProcessInteraction(NPCId, PS);
	}
}

TArray<FActionInfo> UQuestGiverComponent::GetAvailableActions(ASimpleRPGPlayerState* PS) const
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

TArray<FActionInfo> UQuestGiverComponent::GetContextAction(FGameplayTag ActionTag, ASimpleRPGPlayerState* PS) const
{
	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		TArray<FActionInfo> Actions = Subsystem->CreateContextAction(ActionTag, PS);
		for (FActionInfo& Action : Actions)
		{
			Action.ContextOwner = this;
		}
		return Actions;
	}
	return TArray<FActionInfo>();
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

void UQuestGiverComponent::RequestAvailableQuestList()
{

}