// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CharacterPrieviewSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLERPG_API UCharacterPrieviewSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RequestPreview();
	
protected:
	void CapturePreview();

	TObjectPtr<class ULevelStreamingDynamic> StreamingLevel;
};
