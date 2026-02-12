// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GE_Cooldown.h"


void USimpleRPGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability %s does not have montage to play"), *GetName());
		return;
	}

	UAbilityTask_WaitGameplayEvent* WaitAnimEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag("Anim.Event"), nullptr, false, false);
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.f, NAME_None, true, GetRootMotionTranslationScale());

	WaitAnimEventTask->EventReceived.AddDynamic(this, &USimpleRPGGameplayAbility::OnAnimEvent);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USimpleRPGGameplayAbility::OnCompleted);

	WaitAnimEventTask->ReadyForActivation();
	PlayMontageTask->ReadyForActivation();

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void USimpleRPGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USimpleRPGGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	UE_LOG(LogTemp, Log, TEXT("Cancleed"));
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

const FGameplayTagContainer* USimpleRPGGameplayAbility::GetCooldownTags() const
{
	return &CooldownTags;
}

void USimpleRPGGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownDuration > 0.f)
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectContextHandle EffectContext = CurrentActorInfo->AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(ASC->GetAvatarActor());
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UGE_Cooldown::StaticClass(), 1.0f, EffectContext);

			if (SpecHandle.IsValid())
			{
				for (FGameplayTag Tag : CooldownTags)
				{
					SpecHandle.Data->DynamicGrantedTags.AddTag(Tag);
				}
				SpecHandle.Data->SetDuration(CooldownDuration, true);
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void USimpleRPGGameplayAbility::OnExecution()
{

}