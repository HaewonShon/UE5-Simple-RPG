// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackHitModule.generated.h"

/**
 *	Abstract class of attack check Module used in ability
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class SIMPLERPG_API UAttackHitModule : public UObject
{
	GENERATED_BODY()

public:
    UFUNCTION()
    virtual TArray<AActor*> GetHitTargets(AActor* Instigator) PURE_VIRTUAL(UAttackHitModule::GetHitTargets, TArray<AActor*>());

    UFUNCTION()
    virtual void DebugDraw() PURE_VIRTUAL(UAttackHitModule::DebugDraw, );

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShouldDrawDebugInfo;

    UPROPERTY(EditAnywhere, Category = "Debug")
    FColor DebugDrawColor;
};
