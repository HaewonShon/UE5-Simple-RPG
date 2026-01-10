// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestStatusWidget.h"
#include "../Quest/QuestData.h"
#include "Components/TextBlock.h"

void UQuestStatusWidget::RegisterQuest(const UQuestData* QuestData)
{
	Title->SetText(QuestData->Title);
	
	ObjectiveInfos.SetNum(QuestData->Objectives.Num());
	for (int32 i = 0; i < ObjectiveInfos.Num(); ++i)
	{
		const FQuestObjective& Objective = QuestData->Objectives[i];
		ObjectiveInfos[i] = { Objective.TargetDescription, 0, QuestData->Objectives[i].RequiredCount };
	}
	UpdateProgressText();
}

void UQuestStatusWidget::UpdateQuestProgress(int32 ObjectiveIndex, int32 NewProgress)
{
	ObjectiveInfos[ObjectiveIndex].Progress = NewProgress;
	UpdateProgressText();
}

void UQuestStatusWidget::UpdateProgressText()
{
	FTextBuilder Builder;
	for (auto Status : ObjectiveInfos)
	{
		Builder.AppendLine(
			FText::Format(FText::FromString(TEXT("{0}: {1} / {2}")), Status.ObjectiveName, Status.Progress, Status.Goal)
		);
	}
	Progress->SetText(Builder.ToText());
}