// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelComponent.h"

ULevelComponent::ULevelComponent()
{
	CurrentLevel = 1;
	CurrentExp = 0;
}

void ULevelComponent::GrantExp(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentExp += Amount;

	while (CanLevelUp())
	{
		CurrentExp -= GetRequiredExp(CurrentLevel);
		++CurrentLevel;

		OnLevelChanged.Broadcast(CurrentLevel);
	}
	OnExpChanged.Broadcast(CurrentExp, GetRequiredExp(CurrentLevel));
}

bool ULevelComponent::CanLevelUp() const
{
	return CurrentExp >= GetRequiredExp(CurrentLevel);
}

int32 ULevelComponent::GetRequiredExp(int32 Level) const
{
	if (!LevelTable)
	{
		return MAX_int32;
	}

    const FLevelTableRow* Row = LevelTable->FindRow<FLevelTableRow>(FName(*FString::FromInt(CurrentLevel)), TEXT("LevelTable"));

	if (Row)
	{
		return Row->RequiredExp;
	}
	return MAX_int32;
}