// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreviewActor.generated.h"

UCLASS()
class SIMPLERPG_API APreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APreviewActor();

	virtual void BeginPlay() override;

	void Capture();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USceneCaptureComponent2D> SceneCaptureComponent;
};