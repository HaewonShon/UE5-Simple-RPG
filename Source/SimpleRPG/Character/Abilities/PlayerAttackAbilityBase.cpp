// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttackAbilityBase.h"
#include "../PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Gameplay/AttackHitModule/AttackHitModule.h"
#include "../../Enemy/Enemy.h"
#include "../../Gameplay/GE_Damage.h"

// temp for hitmodule creation
#include "../../Gameplay/AttackHitModule/SphereAttackHitModule.h"

void UPlayerAttackAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bCanSetNextCombo = false;
	bNextComboQueued = false;

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->DisableMovement();
		}
	}
}

void UPlayerAttackAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (!bIsFinalCombo && bNextComboQueued)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbilitiesByTag(NextAttackTag.GetSingleTagContainer());
	}
	else
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
			{
				MovementComponent->SetMovementMode(MOVE_Walking);
			}
		}
	}
}

void UPlayerAttackAbilityBase::OnAnimEvent(FGameplayEventData EventData)
{
	Super::OnAnimEvent(EventData);

	FGameplayTag ReceivedTag = EventData.EventTag;
	if (ReceivedTag == FGameplayTag::RequestGameplayTag("Anim.Event.QueueNextCombo"))
	{
		bCanSetNextCombo = true;
	}
	else if (ReceivedTag == FGameplayTag::RequestGameplayTag("Anim.Event.RunNextCombo"))
	{
		if (bNextComboQueued)
		{
			OnCompleted();
		}
		else
		{
			bCanSetNextCombo = false;
		}
	}
}

void UPlayerAttackAbilityBase::SetNextComboFlag(bool flag)
{
	if (bCanSetNextCombo)
	{
		bNextComboQueued = flag;
	}
}

void UPlayerAttackAbilityBase::OnExecution()
{
	if (!AttackHitModule)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Ability %s does not have attack module."), *GetName());
		return;
	}

	TArray<AActor*> HitActors = AttackHitModule->GetHitTargets(CurrentActorInfo->AvatarActor.Get());
	if (HitActors.IsEmpty())
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContext = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(UGE_Damage::StaticClass(), 1.0f, EffectContext);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DamageMultiplier"), DamageMultiplier);

	for (AActor* Actor : HitActors)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (UAbilitySystemComponent* TargetASC = Enemy->GetAbilitySystemComponent())
		{
			if (SpecHandle.IsValid())
			{
				// Set Damage Multiplier from Ability using SetByCaller
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				UE_LOG(LogCharacter, Verbose, TEXT("Hit Enemy %s"), *Enemy->GetName());
			}
		}		
	}
}
