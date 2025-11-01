// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SimpleRPGGameplayAbility.generated.h"

/**
 *	A Base class for all GameplayAbility in this project
 */
UCLASS(BlueprintType)
class SIMPLERPG_API USimpleRPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnAnimEvent(FGameplayEventData EventData);

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	virtual void OnExecution();
	virtual float GetRootMotionTranslationScale() const { return 1.f; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cooldown")
	float CooldownDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;
};
