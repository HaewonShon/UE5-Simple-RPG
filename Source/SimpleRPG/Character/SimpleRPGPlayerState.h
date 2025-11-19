// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
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

	class UAbilitySystemComponent* GetAbilitySystemComponent();
	TWeakObjectPtr<class UInventoryComponent> GetInventoryComponent();
protected:
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<class UAttributeSet> AttributeSet;
	TObjectPtr<class UInventoryComponent> InventoryComponent;
};
