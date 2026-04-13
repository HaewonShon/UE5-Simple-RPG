// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Item/DurationConsumableItem.h"
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

UDurationGE::UDurationGE()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat DurationCaller;
	DurationCaller.DataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Item.Duration"));
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationCaller);

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetAttackPowerAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.AttackPower"));

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetDefenseAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.Defense"));

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetCritChanceAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.CritChance"));

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetMaxHealthAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.MaxHealth"));

	AddSetByCallerModifier(Modifiers,
		UCharacterAttributeSet::GetHealthRegenAttribute(),
		FGameplayTag::RequestGameplayTag("Attribute.HealthRegen"));
}

TSubclassOf<class UGameplayEffect> UDurationConsumableItem::GetGameplayEffectClass() const
{
	return TSubclassOf<class UDurationGE>();
}

bool UDurationConsumableItem::SetGameplayEffectSpecHandleData(FGameplayEffectSpecHandle& SpecHandle) const
{
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Item.Duration"), Duration);

	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.AttackPower"), ItemEffect.AttackPower);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Defense"), ItemEffect.Defense);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.CritChance"), ItemEffect.CritChance);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.MaxHealth"), ItemEffect.MaxHealth);
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.HealthRegen"), ItemEffect.HealthRegen);
	return true;
}