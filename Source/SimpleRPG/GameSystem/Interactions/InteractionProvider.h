// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionProvider.generated.h"

UENUM()
enum class EInteractionType : uint8
{
	Quest,
	Shop,
	Enforcement,
};

USTRUCT()
struct FInteractionInfo
{
	GENERATED_BODY()

	FText DisplayName;
	class UImage* Icon;
	EInteractionType Type;
	// OnExecute
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 *    NPC action(quest, shop, etc) provider
 */
class SIMPLERPG_API IInteractionProvider
{
	GENERATED_BODY()

public:
	virtual TArray<FInteractionInfo> GetAvailableAction(class ASimpleRPGPlayerState* PS) const = 0;
};
