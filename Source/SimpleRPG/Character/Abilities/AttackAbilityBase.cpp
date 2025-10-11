// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAbilityBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "../PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Gameplay/AttackHitModule/AttackHitModule.h"
#include "../../Enemy/Enemy.h"
#include "../../Gameplay/DamageGE.h"

// temp for hitmodule creation
#include "../../Gameplay/AttackHitModule/SphereAttackHitModule.h"

void UAttackAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilityTask_WaitGameplayEvent* QueueComboWaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Anim.Event"), nullptr, false, false);
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);

	QueueComboWaitEventTask->EventReceived.AddDynamic(this, &UAttackAbilityBase::OnAnimEvent);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UAttackAbilityBase::OnCompleted);

	bCanSetNextCombo = false;
	bNextComboQueued = false;

	QueueComboWaitEventTask->ReadyForActivation();
	PlayMontageTask->ReadyForActivation();

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			Character->bUseControllerRotationYaw = false;
			MovementComponent->DisableMovement();
			MovementComponent->bOrientRotationToMovement = false;
		}
	}
}

void UAttackAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bNextComboQueued && !bIsFinalCombo)
	{
		UE_LOG(LogCharacter, Log, TEXT("Combo next called"));
		ActorInfo->AbilitySystemComponent->TryActivateAbilitiesByTag(NextAttackTag.GetSingleTagContainer());
	}
	else
	{
		UE_LOG(LogCharacter, Log, TEXT("Combo next NOT called"));
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
			{
				Character->bUseControllerRotationYaw = true;
				MovementComponent->SetMovementMode(MOVE_Walking);
				MovementComponent->bOrientRotationToMovement = true;
			}
		}
	}
}

void UAttackAbilityBase::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UAttackAbilityBase::OnAnimEvent(FGameplayEventData EventData)
{
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
	else if (ReceivedTag == FGameplayTag::RequestGameplayTag("Anim.Event.OnAttack"))
	{
		OnAttack();
	}
}

void UAttackAbilityBase::SetNextComboFlag(bool flag)
{
	if (bCanSetNextCombo)
	{
		bNextComboQueued = flag;
	}
}

void UAttackAbilityBase::OnAttack()
{
	if (!AttackHitModule)
	{
		UE_LOG(LogCharacter, Warning, TEXT("Ability %s does not have attack module."), *GetName());
		return;
	}

	TArray<AActor*> HitActors = AttackHitModule->GetHitTargets(CurrentActorInfo->OwnerActor.Get());
	if (HitActors.IsEmpty())
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();
	FGameplayEffectContextHandle EffectContext = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());

	for (AActor* Actor : HitActors)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (UAbilitySystemComponent* TargetASC = Enemy->GetAbilitySystemComponent())
		{
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(UDamageGE::StaticClass(), 1.0f, EffectContext);
			if (SpecHandle.IsValid())
			{
				// Set Damage Multiplier from Ability using SetByCaller
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DamageMultiplier"), DamageMultiplier);
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				UE_LOG(LogCharacter, Log, TEXT("Hit Enemy"));
			}
		}		
	}
}
