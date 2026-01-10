// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestStatusWidget.generated.h"

USTRUCT()
struct FQuestObjectiveInfo
{
	GENERATED_BODY()
	
	FText ObjectiveName;
	int32 Progress;
	int32 Goal;
};

/**
 *  Widget for displaying current quests, including title and progress
 */
UCLASS()
class SIMPLERPG_API UQuestStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void RegisterQuest(const class UQuestData* QuestData);
	void UpdateQuestProgress(int32 ObjectiveIndex, int32 NewProgress);

protected:
	void UpdateProgressText();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Title;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Progress;

	TArray<FQuestObjectiveInfo> ObjectiveInfos;
};
