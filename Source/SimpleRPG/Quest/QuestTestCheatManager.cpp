// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTestCheatManager.h"
#include "QuestManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/SimpleRPGPlayerState.h"

void UQuestTestCheatManager::GiveQuest(int32 Index)
{
	if (Index >= QuestList.Num())
	{
		return;
	}

	ensure(GetWorld());

	if (UQuestManagerSubsystem* QuestManagerSubsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		QuestManagerSubsystem->GrantQuest(QuestList[Index]->AssetId, Cast<ASimpleRPGPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)));
	}
}