// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffect.h"
#include "EffectDurationWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWidgetHovered);

USTRUCT(BlueprintType)
struct FTimedEffectDisplayData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveEffectHandle;
};

/**
 *		widget to effect's duration on hud
 */

UCLASS()
class SIMPLERPG_API UEffectDurationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnWidgetHovered OnHovered;

protected:
};