// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeSet.h"

UCharacterAttributeSet::UCharacterAttributeSet()
{
}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.f, GetMaxHealthAttribute().GetNumericValue(this));
	}
	else if (Attribute == GetCritChanceAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.f, 100.f);
	}
	else
	{
		// prevent negative value
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
