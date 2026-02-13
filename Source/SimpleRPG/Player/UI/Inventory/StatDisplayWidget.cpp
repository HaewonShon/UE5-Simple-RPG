// Fill out your copyright notice in the Description page of Project Settings.


#include "StatDisplayWidget.h"
#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Player/SimpleRPGPlayerState.h"

void UStatDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/*if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		AbilitySystemComponent = PlayerState->GetAbilitySystemComponentWeakPtr();
		check(AbilitySystemComponent.IsValid());


	}*/
}

void UStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		check(ASC);

		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UStatWidget::OnAttributeValueChanged);

		bool bHasAttribute;
		float Value = ASC->GetGameplayAttributeValue(Attribute, bHasAttribute);
		check(bHasAttribute);

		FText Content = FText::Format(FText::FromString("{0} : {1}"), DisplayName, FText::AsNumber(Value));
		TextBlock->SetText(Content);
	}
}

void UStatWidget::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	float Value = Data.NewValue;

	FText Content = FText::Format(FText::FromString("{0} : {1}"), DisplayName, FText::AsNumber(Value));
	TextBlock->SetText(Content);
}
