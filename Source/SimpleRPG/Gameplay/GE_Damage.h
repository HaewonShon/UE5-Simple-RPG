// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Damage.generated.h"

/**
 *  GE for applying damage multiplier
 */
UCLASS()
class SIMPLERPG_API UGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UGE_Damage();
};
