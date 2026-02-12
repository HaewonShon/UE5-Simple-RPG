// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/GameAbilitySystem/SimpleRPGGameplayAbility.h"
#include "PlayerAttackAbilityBase.generated.h"

/**
 *	Base class For Player Attack Abilities
 *  Support flexible combo attack system by FGameplayTag, Damage Application based on DamageMultiplier
 */
UCLASS()
class SIMPLERPG_API UPlayerAttackAbilityBase : public USimpleRPGGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
	virtual void OnAnimEvent(FGameplayEventData EventData);
	
	void SetNextComboFlag(bool flag);

protected:
	virtual void OnExecution();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float DamageMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<class UAttackHitModule> AttackHitModule;

	// Optional - tag 구분하여 event 처리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	FGameplayTag NextAttackTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	bool bIsFinalCombo;

	bool bCanSetNextCombo;
	
	bool bNextComboQueued;
};
