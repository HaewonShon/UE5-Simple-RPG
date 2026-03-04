// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SlotTypes.h"
#include "ActionableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActionableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *		Interface for actionable assets, such as item use or skill execution
 */
class SIMPLERPG_API IActionableInterface
{
	GENERATED_BODY()

public:
	virtual bool CanExecute(AActor* Executer, const FSlotAddress& SourceAddress) const;
	virtual void Execute(AActor* Executer, const FSlotAddress& SourceAddress);
};
