// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_GreatSwordComboAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "../PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_GreatSwordComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilityTask_WaitGameplayEvent* QueueComboWaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Anim.Event"), nullptr, false, false);
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);

	QueueComboWaitEventTask->EventReceived.AddDynamic(this, &UGA_GreatSwordComboAttack::OnAnimEvent);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_GreatSwordComboAttack::OnCompleted);

	bCanSetNextCombo = false;
	bNextComboQueued = false;

	QueueComboWaitEventTask->ReadyForActivation();
	PlayMontageTask->ReadyForActivation();

	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->GetCharacterMovement()->DisableMovement();
	}
}

void UGA_GreatSwordComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}

void UGA_GreatSwordComboAttack::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_GreatSwordComboAttack::OnAnimEvent(FGameplayEventData EventData)
{
	FGameplayTag ReceivedTag = EventData.EventTag;

	if (ReceivedTag == FGameplayTag::RequestGameplayTag("Anim.Event.QueueNextCombo"))
	{
		bCanSetNextCombo = true;
		UE_LOG(LogCharacter, Log, TEXT("Can set next combo"));
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
		// Trigger Attack
	}
}

void UGA_GreatSwordComboAttack::SetNextComboFlag(bool flag)
{
	if (bCanSetNextCombo)
	{
		UE_LOG(LogCharacter, Log, TEXT("Set next combo"));
		bNextComboQueued = flag;
	}
}
