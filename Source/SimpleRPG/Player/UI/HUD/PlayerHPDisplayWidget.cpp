// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHPDisplayWidget.h"
#include "Player/PlayerCharacter.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"

void UPlayerHPDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		{
			FGameplayAttribute HealthAttribute = UCharacterAttributeSet::GetHealthAttribute();
			ASC->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &UPlayerHPDisplayWidget::OnHealthChanged);

			float MaxHealth = ASC->GetNumericAttribute(UCharacterAttributeSet::GetMaxHealthAttribute());
			SetMaxValue(MaxHealth);
		}
	}
}

void UPlayerHPDisplayWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	UpdateCurrentValue(Health);
}