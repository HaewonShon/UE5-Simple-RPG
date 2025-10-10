// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackHitModule.h"
#include "SphereAttackHitModule.generated.h"

/**
 *  Sphere-Shpae attack check module
 */
UCLASS()
class SIMPLERPG_API USphereAttackHitModule : public UAttackHitModule
{
	GENERATED_BODY()

public:
    virtual TArray<AActor*> GetHitTargets(AActor* Instigator) override;

    virtual void DebugDraw() override;

    UPROPERTY(EditAnywhere, Category = "Hit")
    float Range;

    UPROPERTY(EditAnywhere, Category = "Hit")
    float Radius;
};
