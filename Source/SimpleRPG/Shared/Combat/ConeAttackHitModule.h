// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackHitModule.h"
#include "ConeAttackHitModule.generated.h"

/**
 * Cone-shape attack check module
 */
UCLASS()
class SIMPLERPG_API UConeAttackHitModule : public UAttackHitModule
{
	GENERATED_BODY()

public:
    virtual TArray<AActor*> GetHitTargets(AActor* Instigator) override;

    void DebugDraw(const FVector& Location, const FVector& Forward);

    UPROPERTY(EditAnywhere, Category = "Hit")
    float Range;

    UPROPERTY(EditAnywhere, Category = "Hit")
    float Angle;
	
private:
    bool IsInside(AActor* Target, const FVector& Location, const FVector& Forward) const;
};
