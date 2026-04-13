// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "GameplayTagContainer.h"
#include "ConsumableItemData.generated.h"

/**
 *	Data Asset for consumable
 */
UCLASS(Abstract, BlueprintType, meta = (DisplayName = "Consumable Item Data Asset"))
class SIMPLERPG_API UConsumableItemData : public UItemData
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(AActor* Executer, const FSlotAddress& Address) const override;
	virtual void Execute(AActor* Executer, const FSlotAddress& Address) override;

	virtual FItemDescription BuildDescriptionData() const override;

	virtual TSubclassOf<class UGameplayEffect> GetGameplayEffectClass() const { return nullptr; }
	virtual bool SetGameplayEffectSpecHandleData(struct FGameplayEffectSpecHandle& SpecHandle) const { return false; }
};
