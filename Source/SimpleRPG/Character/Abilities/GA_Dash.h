// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Gameplay/SimpleRPGGameplayAbility.h"
#include "GA_Dash.generated.h"

/**
 *	Ability for Player Dash
 */
UCLASS()
class SIMPLERPG_API UGA_Dash : public USimpleRPGGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	virtual float GetRootMotionTranslationScale() const override { return 1.f; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	FVector DashPower;
};
