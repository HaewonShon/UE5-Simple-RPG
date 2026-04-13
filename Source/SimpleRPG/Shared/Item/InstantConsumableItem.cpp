// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Item/InstantConsumableItem.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"

namespace
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

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetHealthAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.Health"));
}

TSubclassOf<class UGameplayEffect> UInstantConsumableItem::GetGameplayEffectClass() const
{
	return TSubclassOf<class UInstantGE>();
}

bool UInstantConsumableItem::SetGameplayEffectSpecHandleData(FGameplayEffectSpecHandle& SpecHandle) const
{
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Health"), ItemEffect.Health);
	return true;
}