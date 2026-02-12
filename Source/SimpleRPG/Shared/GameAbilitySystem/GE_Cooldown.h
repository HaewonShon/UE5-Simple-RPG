// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Cooldown.generated.h"

/**
 *  GE for applying cooldown for ability
 */
UCLASS()
class SIMPLERPG_API UGE_Cooldown : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_Cooldown();
};
