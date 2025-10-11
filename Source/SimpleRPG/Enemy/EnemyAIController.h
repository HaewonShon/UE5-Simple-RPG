// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 *  Controller for AI Enemy for running BT
 */
UCLASS()
class SIMPLERPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
    virtual void OnPossess(APawn* InPawn) override;

private:
    UPROPERTY(EditDefaultsOnly)
    class UBehaviorTree* BehaviorTreeAsset;
};
