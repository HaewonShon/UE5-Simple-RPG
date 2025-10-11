// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameplayEffectTypes.h"
#include "EnemyHPDisplayWidgetComponent.generated.h"

/**
 *	Widget Component displays enemy HP
 */
UCLASS(BlueprintType)
class SIMPLERPG_API UEnemyHPDisplayWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UEnemyHPDisplayWidgetComponent();

	virtual void BeginPlay() override;

	void OnHealthChanged(const FOnAttributeChangeData& Data);

protected:
	TWeakObjectPtr<class UProgressDisplayWidget> DisplayWidget;
};
