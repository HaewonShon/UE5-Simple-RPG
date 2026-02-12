// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "DialogueCameraActor.generated.h"

/*
*	Spawn Camera on back-leftside of the player
*/
UCLASS()
class SIMPLERPG_API ADialogueCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
public:
	void SetupCameraTransform(const FVector& PlayerPosition, const FVector& NPCPosition);
	void ActivateCamera(APlayerController* PlayerController, float BlendTime = 0.f);
	void DeactivateCamera(AActor* PlayerCharacter, APlayerController* PlayerController, float BlendTime = 0.f);

protected:
	UPROPERTY(EditAnywhere, Category = "Dialogue Camera")
	float CameraDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Dialogue Camera")
	float CameraHeight = 150.f;

	UPROPERTY(EditAnywhere, Category = "Dialogue Camera")
	float RightOffset = -100.f;
};
