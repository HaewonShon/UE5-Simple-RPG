// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "QuestData.h"
#include "QuestTestCheatManager.generated.h"

/**
 *	Cheat class for quest test
 */
UCLASS()
class SIMPLERPG_API UQuestTestCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	UFUNCTION(exec)
	void GiveQuest(int32 Index);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UQuestData*> QuestList;
};
