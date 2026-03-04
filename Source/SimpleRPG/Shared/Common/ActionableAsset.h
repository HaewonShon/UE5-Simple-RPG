// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActionableInterface.h"
#include "ActionableAsset.generated.h"

/**
 *		base for actionable(executable) assets, including skills, items
 */
UCLASS(Abstract)
class SIMPLERPG_API UActionableAsset : public UPrimaryDataAsset, public IActionableInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	virtual bool CanExecute(AActor* Executer, const FSlotAddress& SourceAddress) const override { return false; };
	virtual void Execute(AActor* Executer, const FSlotAddress& SourceAddress) override {};
};
