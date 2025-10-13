// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AttackAbilityBase.generated.h"

/**
 *	Base class For Attack Abilities
 *  Support flexible combo attack system by FGameplayTag, Damage Application based on DamageMultiplier
 */
UCLASS()
class SIMPLERPG_API UAttackAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	virtual void OnCompleted();	
	UFUNCTION()
	virtual void OnAnimEvent(FGameplayEventData EventData);
	
	void SetNextComboFlag(bool flag);

protected:
	virtual void OnAttack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Animation")
	TObjectPtr<UAnimMontage> MontageToPlay;

	// Optional - tag 구분하여 event 처리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	FGameplayTag NextAttackTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float DamageMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<class UAttackHitModule> AttackHitModule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool bIsFinalCombo;

	bool bCanSetNextCombo;
	bool bNextComboQueued;
};
