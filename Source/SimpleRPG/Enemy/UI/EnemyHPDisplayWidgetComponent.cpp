// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPDisplayWidgetComponent.h"
#include "../Enemy.h"
#include "Shared/UI/Common/ProgressDisplayWidget.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"
#include "Kismet/GameplayStatics.h"

UEnemyHPDisplayWidgetComponent::UEnemyHPDisplayWidgetComponent()
{
	if (!GetWidgetClass())
	{
		SetWidgetClass(UProgressDisplayWidget::StaticClass());
	}
}

void UEnemyHPDisplayWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CamManager.IsValid())
	{
		return;
	}
	FRotator LookAt = CamManager->GetCameraRotation();
	LookAt.Add(0.f, 180.f, 0.f);
	LookAt.Pitch = 360.f - LookAt.Pitch;
	SetWorldRotation(LookAt);
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

		// get local camera to track
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			CamManager = PC->PlayerCameraManager.Get();
		}
	}
}

void UEnemyHPDisplayWidgetComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	DisplayWidget->UpdateCurrentValue(Health);
}