// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "Item/ItemData.h"
#include "ItemTestCheatManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCheat, Log, All)

/**
 *   Cheat Class for item test
 */
UCLASS()
class SIMPLERPG_API UItemTestCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(exec)
	void GiveItem(int32 Index);

protected:
	TWeakObjectPtr<class UInventoryComponent> GetPlayerInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UItemData*> ItemList;
};
