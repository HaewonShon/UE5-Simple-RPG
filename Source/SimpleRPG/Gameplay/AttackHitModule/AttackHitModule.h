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
    UAttackHitModule();

    UFUNCTION()
    virtual TArray<AActor*> GetHitTargets(AActor* Instigator) PURE_VIRTUAL(UAttackHitModule::GetHitTargets, return TArray<AActor*>(); );

    UPROPERTY(EditAnywhere, Category = "Combat")
    TEnumAsByte<ECollisionChannel> ChannelToHit;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<APawn> TargetClass;

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShouldDrawDebugInfo;

    UPROPERTY(EditAnywhere, Category = "Debug")
    FColor DebugDrawColor;

};
