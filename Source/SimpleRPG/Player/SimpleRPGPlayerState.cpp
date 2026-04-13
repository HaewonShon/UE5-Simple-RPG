// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerState.h"
#include "Shared/GameAbilitySystem/CharacterAttributeSet.h"
#include "Components/InventoryComponent.h"
#include "Components/CurrencyComponent.h"
#include "Components/LevelComponent.h"
#include "Components/EquipmentComponent.h"
#include "Interaction/Quest/QuestManagerComponent.h"
#include "Shared/UI/UISubsystem.h"

ASimpleRPGPlayerState::ASimpleRPGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet.Get());

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	QuestManagerComponent = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("QuestManagerComponent"));
	CurrencyComponent = CreateDefaultSubobject<UCurrencyComponent>(TEXT("CurrencyComponent"));
	LevelComponent = CreateDefaultSubobject<ULevelComponent>(TEXT("LevelComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}

void ASimpleRPGPlayerState::BeginPlay()
{
	Super::BeginPlay();
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

UUISubsystem* ASimpleRPGPlayerState::GetUISubsystem() const
{
	// Local player check
	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController && PlayerController->IsLocalController())
	{
		// Controller -> LocalPlayer -> Subsystem
		if (ULocalPlayer* LP = PlayerController->GetLocalPlayer())
		{
			return LP->GetSubsystem<UUISubsystem>();
		}
	}
	UE_LOG(LogPlayerController, Error, TEXT("Failed to get local UISubsystem"));
	return nullptr;
	
}

void ASimpleRPGPlayerState::NotifyEnemyKilled(FGameplayTag EnemyTag)
{
	QuestManagerComponent->OnEnemyKilled(EnemyTag);
}
