// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


void USimpleRPGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability %s does not have montage to play"), *GetName());
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitAnimEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Anim.Event"), nullptr, false, false);
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);

	WaitAnimEventTask->EventReceived.AddDynamic(this, &USimpleRPGGameplayAbility::OnAnimEvent);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USimpleRPGGameplayAbility::OnCompleted);

	WaitAnimEventTask->ReadyForActivation();
	PlayMontageTask->ReadyForActivation();
}

void USimpleRPGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USimpleRPGGameplayAbility::OnCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void USimpleRPGGameplayAbility::OnAnimEvent(FGameplayEventData EventData)
{
	FGameplayTag ReceivedTag = EventData.EventTag;

	if (ReceivedTag == FGameplayTag::RequestGameplayTag("Anim.Event.OnExecute"))
	{
		OnExecution();
	}
}

void USimpleRPGGameplayAbility::OnExecution()
{

}