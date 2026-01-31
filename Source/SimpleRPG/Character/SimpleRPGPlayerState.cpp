// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerState.h"
#include "../Gameplay/CharacterAttributeSet.h"
#include "Components/Inventory/InventoryComponent.h"
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
	InventoryComponent->SetAbilitySystemComponentRef(AbilitySystemComponent);
	QuestManagerComponent->SetInventoryComponentRef(InventoryComponent);
}

UAbilitySystemComponent* ASimpleRPGPlayerState::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

TWeakObjectPtr<class UInventoryComponent> ASimpleRPGPlayerState::GetInventoryComponent()
{
	return InventoryComponent;
}

TWeakObjectPtr<class UQuestManagerComponent> ASimpleRPGPlayerState::GetQuestManagerComponent()
{
	return QuestManagerComponent;
}

void ASimpleRPGPlayerState::NotifyEnemyKilled(FGameplayTag EnemyTag)
{
	QuestManagerComponent->OnEnemyKilled(EnemyTag);
}
