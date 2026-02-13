// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DropActorData.generated.h"

/**
 *	 Asset for item spawn subsystem
 */
UCLASS()
class SIMPLERPG_API UDropActorData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("DropActorData", GetFName()); }

	UPROPERTY(EditDefaultsOnly, Category = "Drop Actor")
	TSubclassOf<class AGoldDropActor> GoldDropActor;

	UPROPERTY(EditDefaultsOnly, Category = "Drop Actor")
	TSubclassOf<class AItemActor> ItemActor;
};
