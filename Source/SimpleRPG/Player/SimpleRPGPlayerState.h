// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "SimpleRPGPlayerState.generated.h"

/**
 *		Player State for the game, responsible for GAS components and Inventory
 */
UCLASS()
class SIMPLERPG_API ASimpleRPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASimpleRPGPlayerState();

	virtual void BeginPlay();

	class UAbilitySystemComponent* GetAbilitySystemComponent();
	TWeakObjectPtr<class UInventoryComponent> GetInventoryComponent();
	TWeakObjectPtr<class UQuestManagerComponent> GetQuestManagerComponent();

	class UUISubsystem* GetUISubsystem() const;

	void NotifyEnemyKilled(FGameplayTag EnemyTag);
protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UQuestManagerComponent> QuestManagerComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UCurrencyComponent> CurrencyComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class ULevelComponent> LevelComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UEquipmentComponent> EquipmentComponent;
};
