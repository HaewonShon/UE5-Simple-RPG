// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerState.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "Inventory/InventoryComponent.h"
#include "../Quest/QuestManagerComponent.h"

ASimpleRPGPlayerState::ASimpleRPGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	QuestManagerComponent = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("QuestManagerComponent"));
}

void ASimpleRPGPlayerState::BeginPlay()
{
	Super::BeginPlay();
	InventoryComponent->SetAbilitySystemComponent(AbilitySystemComponent);
}

UAbilitySystemComponent* ASimpleRPGPlayerState::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

TWeakObjectPtr<class UInventoryComponent> ASimpleRPGPlayerState::GetInventoryComponent()
{
	return InventoryComponent;
}

void ASimpleRPGPlayerState::NotifyEnemyKilled(FGameplayTag EnemyTag)
{
	QuestManagerComponent->OnEnemyKilled(EnemyTag);
}
