// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/ProgressDisplayWidget.h"
#include "GameplayEffectTypes.h"
#include "PlayerHPDisplayWidget.generated.h"

/**
 *	HP Display Widget for player
 */
UCLASS(Blueprintable)
class SIMPLERPG_API UPlayerHPDisplayWidget : public UProgressDisplayWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;	

	void OnHealthChanged(const FOnAttributeChangeData& Data);
};
