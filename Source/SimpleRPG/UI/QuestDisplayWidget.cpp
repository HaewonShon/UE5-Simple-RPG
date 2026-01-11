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
	}
}

void UQuestDisplayWidget::RegisterQuest(const class UQuestData* Quest)
{
	UQuestStatusWidget* StatusWidget = CreateWidget<UQuestStatusWidget>(GetOwningPlayer(), StatusWidgetClass.Get());
	StatusWidgetSlot->AddChildToVerticalBox(StatusWidget);
}