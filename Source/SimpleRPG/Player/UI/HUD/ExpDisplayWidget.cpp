// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpDisplayWidget.h"
#include "Player/Components/LevelComponent.h"
#include "Player/SimpleRPGPlayerState.h"

void UExpDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		if (ULevelComponent* LevelComponent = PlayerState->GetComponentByClass<ULevelComponent>())
		{
			LevelComponent->OnExpChanged.AddUObject(this, &UExpDisplayWidget::OnExpChanged);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to setup exp display widget"));
}

void UExpDisplayWidget::OnExpChanged(int32 NewExp, int32 MaxExp)
{
	SetMaxValue(MaxExp, false, false);
	UpdateCurrentValue(NewExp);
}