// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionRotationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UInteractionRotationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionRotationComponent();
	void StartRoationToTarget(AActor* Target);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FRotator DefaultRotation;
	FRotator TargetRotation;
	FTimerHandle RotationTimerHandle;
	float RotationInterpSpeed = 10.f;
	float AcceptableYawDiff = 1.0f;
	void UpdateRotationToTarget();
};
