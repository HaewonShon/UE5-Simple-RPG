// Fill out your copyright notice in the Description page of Project Settings.


#include "PortraitDisplayWidget.h"
#include "../../Character/Components/LevelComponent.h"
#include "../../Character/SimpleRPGPlayerState.h"
#include "../Gameplay/CharacterPrieviewSubsystem.h"
#include "Components/TextBlock.h"

void UPortraitDisplayWidget::NativeConstruct()
{
	// portrait Setup
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (UCharacterPrieviewSubsystem* Subsystem = World->GetSubsystem<UCharacterPrieviewSubsystem>())
	{
		UE_LOG(LogTemp, Log, TEXT("UPortraitDisplayWidget Preview requested"));
		Subsystem->RequestPreview();
	}

	// Level display setup
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		if (ULevelComponent* LevelComponent = PlayerState->GetComponentByClass<ULevelComponent>())
		{
			LevelComponent->OnLevelChanged.AddUObject(this, &UPortraitDisplayWidget::OnLevelChanged);
			return;
		}
	}
}

void UPortraitDisplayWidget::OnLevelChanged(int32 NewLevel)
{
	LevelText->SetText(FText::AsNumber(NewLevel));
}