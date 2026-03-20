// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DropActor.h"
#include "GoldDropActor.generated.h"

UCLASS()
class SIMPLERPG_API AGoldDropActor : public ADropActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoldDropActor();

	void SetGoldAmount(int32 Amount);

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditDefaultsOnly, Category = "Gold")
	FLinearColor VFXColor;

	UPROPERTY(EditAnywhere)
	int32 GoldAmount;
};
