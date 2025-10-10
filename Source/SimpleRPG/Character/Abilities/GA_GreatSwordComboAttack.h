// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_GreatSwordComboAttack.generated.h"

/**
 *	Base class For Melee Attack Combo Ability
 *  Melee Attack 1, 2, 3 will derive this class
 */
UCLASS()
class SIMPLERPG_API UGA_GreatSwordComboAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	//virtual void ActivateAbility() override;
	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnAnimEvent(FGameplayEventData EventData);

	void SetNextComboFlag(bool flag);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animation")
	UAnimMontage* MontageToPlay;

	// Optional - tag 구분하여 event 처리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FGameplayTag NextAttackTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool bIsFinalCombo;

	bool bCanSetNextCombo;
	bool bNextComboQueued;
};
