// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SimpleRPGGameInstance.generated.h"

/**
 *	GameInstance for the game. Includes game-related data
 */
UCLASS()
class SIMPLERPG_API USimpleRPGGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	TObjectPtr<UDataTable> GetLootTable() { return LootTable; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UDataTable> LootTable;
};\
