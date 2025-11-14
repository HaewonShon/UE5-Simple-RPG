// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerState.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "Inventory/InventoryComponent.h"

ASimpleRPGPlayerState::ASimpleRPGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

UAbilitySystemComponent* ASimpleRPGPlayerState::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}