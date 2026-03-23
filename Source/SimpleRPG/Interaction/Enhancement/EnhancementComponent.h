// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/ActionProvider.h"
#include "EnhancementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEnhancement, Log, All)
DECLARE_MULTICAST_DELEGATE(FOnEnhancementFisnihed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UEnhancementComponent : public UActorComponent, public IActionProvider
{
	GENERATED_BODY()

public:	
	UEnhancementComponent();

	/* Action Provider Interface */
	virtual TArray<FActionInfo> CreateAvailableActions(class ASimpleRPGPlayerState* PS);
	virtual FActionInfo CreateContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS);
};
