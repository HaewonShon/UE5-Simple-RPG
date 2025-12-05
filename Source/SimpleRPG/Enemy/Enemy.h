// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Enemy.generated.h"

// Define Log Category for enemy-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogEnemy, Log, All)

UCLASS()
class SIMPLERPG_API AEnemy : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController);

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	/*******************************************
	*	GAMEPLAY ABILITIY SYSTEM
	*******************************************/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void InitializeAttributes();
	void AddCharacterAbilities();

	void OnDeath();

	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy")
	FGameplayTag EnemyTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TObjectPtr<class UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> OwningAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> DeathAnimMontage;

	/*******************************************
	*	UI
	*******************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TObjectPtr<class UEnemyHPDisplayWidgetComponent> HPDisplayWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class ADamageTextActor> DamageTextClass;
};
