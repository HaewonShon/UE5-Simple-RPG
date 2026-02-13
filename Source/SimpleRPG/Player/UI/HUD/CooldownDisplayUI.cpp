// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownDisplayUI.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerCharacter.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"


void UCooldownDisplayUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		{
			ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UCooldownDisplayUI::OnCooldownApplied);
			UE_LOG(LogCharacter, Verbose, TEXT("Cooldown ui setup"));
		}
	}

	CooldownInitial = 0.f;
	if (ProgressBar)
	{
		ProgressBar->SetPercent(0.f);
	}

}

void UCooldownDisplayUI::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);

	if (CooldownProgress > 0.f)
	{
		CooldownProgress -= InDeltaTime;

		// on skill available
		if (CooldownProgress < 0.f)
		{
			if (ProgressBar)
			{
				ProgressBar->SetPercent(0.f);
			}
			if (Borderline)
			{
				//Border->SetOpacity(1.f);
			}
		}
		else
		{
			if (ProgressBar)
			{
				ProgressBar->SetPercent(CooldownProgress / CooldownInitial);
			}
		}
	}
}

void UCooldownDisplayUI::OnCooldownApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	if (ActiveHandle.IsValid() && SpecApplied.DynamicGrantedTags.HasTag(CooldownTag))
	{
		UE_LOG(LogCharacter, Verbose, TEXT("Cooldown status changed. %f"), SpecApplied.GetDuration());

		CooldownInitial = SpecApplied.GetDuration();
		CooldownProgress = CooldownInitial;

		if (ProgressBar)
		{
			ProgressBar->SetPercent(1.f);
		}
	}
}

void UCooldownDisplayUI::PlaySound()
{
    if (CooldownReadySound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), CooldownReadySound, 1.0f, 1.0f, 0.0f);
    }
}
