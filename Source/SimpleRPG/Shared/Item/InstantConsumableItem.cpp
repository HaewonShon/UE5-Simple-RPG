// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Item/InstantConsumableItem.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"

namespace InstantUtility
{
	void AddSetByCallerModifier(
		TArray<FGameplayModifierInfo>& Modifiers,
		const FGameplayAttribute& Attribute,
		const FGameplayTag& DataTag,
		EGameplayModOp::Type ModifierOp = EGameplayModOp::Additive)
	{
		FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();
		Modifier.Attribute = Attribute;
		Modifier.ModifierOp = ModifierOp;

		FSetByCallerFloat Caller;
		Caller.DataTag = DataTag;

		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Caller);
	}
}

UInstantGE::UInstantGE()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// TODO : Change hardcoded part(Health only current)
	FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();
	Modifier.Attribute = UCharacterAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat Caller;
	Caller.DataTag = FGameplayTag::RequestGameplayTag("Attribute.Health");

	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Caller);
}

TSubclassOf<class UGameplayEffect> UInstantConsumableItem::GetGameplayEffectClass() const
{
	return UInstantGE::StaticClass();
}

bool UInstantConsumableItem::SetGameplayEffectSpecHandleData(FGameplayEffectSpecHandle& SpecHandle) const
{
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Health"), ItemEffect.Health);
	return true;
}