// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CooldownDisplayUI.generated.h"

/**
 *    UI for displaying cooldown for ability
 */
UCLASS()
class SIMPLERPG_API UCooldownDisplayUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;

protected:
	// This function checks applied GE of owner and check cooldown tag.
	UFUNCTION()
	void OnCooldownApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	void PlaySound();

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UImage> Icon;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UBorder> Borderline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cooldown")
	TObjectPtr<class USoundBase> CooldownReadySound;

	float CooldownProgress;

	float CooldownInitial;
};
