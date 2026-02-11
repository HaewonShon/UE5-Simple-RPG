// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "ActionProvider.generated.h"

DECLARE_DELEGATE(FOnActionExecuted)

UENUM()
enum class EActionType : uint8
{
	QuestSelect,
	QuestAccept,
	QuestDecline,
	Shop,
	Enforcement,
};

USTRUCT()
struct FActionInfo
{
	GENERATED_BODY()

	FText DisplayName;
	class UTexture2D* Icon;
	EActionType Type;
	FOnActionExecuted OnActionExecuted;
	const class UActorComponent* ContextOwner;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActionProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 *    NPC action(quest, shop, etc) provider
 */
class SIMPLERPG_API IActionProvider
{
	GENERATED_BODY()

public:
	virtual TArray<FActionInfo> GetAvailableActions(class ASimpleRPGPlayerState* PS) const = 0;
	virtual TArray<FActionInfo> GetContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS) const = 0;
};