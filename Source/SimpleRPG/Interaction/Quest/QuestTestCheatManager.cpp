// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTestCheatManager.h"
#include "QuestManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SimpleRPGPlayerState.h"

void UQuestTestCheatManager::GiveQuest(int32 Index)
{
	if (Index >= QuestList.Num())
	{
		return;
	}

	ensure(GetWorld());

	if (UQuestManagerSubsystem* QuestManagerSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		//QuestManagerSubsystem->TryGrantQuest(QuestList[Index]->AssetId, Cast<ASimpleRPGPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)));
	}
}