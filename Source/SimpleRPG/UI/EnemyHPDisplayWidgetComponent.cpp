// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPDisplayWidgetComponent.h"
#include "ProgressDisplayWidget.h"
#include "../Enemy/Enemy.h"
#include "../Gameplay/CharacterAttributeSet.h"

UEnemyHPDisplayWidgetComponent::UEnemyHPDisplayWidgetComponent()
{
    if (!GetWidgetClass())
    {
        SetWidgetClass(UProgressDisplayWidget::StaticClass());
    }
}

void UEnemyHPDisplayWidgetComponent::BeginPlay()
{
    Super::BeginPlay();

    DisplayWidget = Cast<UProgressDisplayWidget>(GetUserWidgetObject());
    if (DisplayWidget.IsValid())
    {
        if (AEnemy* Enemy = Cast<AEnemy>(GetOwner()))
        {
            if (UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent())
            {
                FGameplayAttribute HealthAttribute = UCharacterAttributeSet::GetHealthAttribute();
                ASC->GetGameplayAttributeValueChangeDelegate(HealthAttribute).AddUObject(this, &UEnemyHPDisplayWidgetComponent::OnHealthChanged);

                float MaxHealth = ASC->GetNumericAttribute(UCharacterAttributeSet::GetMaxHealthAttribute());
                DisplayWidget->SetMaxValue(MaxHealth);
                UE_LOG(LogEnemy, Log, TEXT("Enemy HPBar Widget registered"));
            }
        }
    }
}

void UEnemyHPDisplayWidgetComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float Health = Data.NewValue;
    UE_LOG(LogEnemy, Log, TEXT("UEnemyHPDisplayWidgetComponent OnHealthChanged Called, health: %f"), Health);
    DisplayWidget->UpdateCurrentValue(Health);
}