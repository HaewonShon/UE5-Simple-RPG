// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestDisplayWidget.h"
#include "../Character/SimpleRPGPlayerState.h"
#include "../Quest/QuestManagerComponent.h"
#include "Components/VerticalBox.h"
#include "QuestStatusWidget.h"

void UQuestDisplayWidget::NativeConstruct()
{
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		// Register components from PS
		UQuestManagerComponent* QuestManagerComponent = PlayerState->GetQuestManagerComponent().Get();
		check(QuestManagerComponent);

		QuestManagerComponent->OnQuestAccepted.BindUObject(this, &UQuestDisplayWidget::RegisterQuest);
		QuestManagerComponentRef = QuestManagerComponent;
	}
}

void UQuestDisplayWidget::RegisterQuest(const class UQuestData* Quest)
{
	UQuestStatusWidget* StatusWidget = CreateWidget<UQuestStatusWidget>(GetOwningPlayer(), StatusWidgetClass.Get());
	if (StatusWidget)
	{
		StatusWidget->RegisterQuest(Quest);
		StatusWidgetSlot->AddChildToVerticalBox(StatusWidget);

		QuestManagerComponentRef->OnQuestProgressChanged.AddUObject(this, &UQuestDisplayWidget::UpdateQuestProgress);

		StatusWidgetMap.Add({Quest->AssetId, StatusWidget});
	}
}

void UQuestDisplayWidget::UpdateQuestProgress(FPrimaryAssetId QuestId, int32 ObjectiveIndex, int32 Progress)
{
	if (UQuestStatusWidget* StatusWidget = StatusWidgetMap[QuestId])
	{
		StatusWidget->UpdateQuestProgress(ObjectiveIndex, Progress);
	}
	else
	{
		UE_LOG(LogQuest, Warning, TEXT("Display Widget could not find quest with id: %s"), *QuestId.ToString());
	}
}
