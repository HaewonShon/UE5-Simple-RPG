// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestIconData.generated.h"

/**
 *	Icon holders for quest subsystem
 */
UCLASS()
class SIMPLERPG_API UQuestIconData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("QuestIconData", GetFName()); }

	UPROPERTY(EditDefaultsOnly, Category="Quest")
	TObjectPtr<class UTexture2D> QuestAvailableIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TObjectPtr<class UTexture2D> QuestInProgressIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TObjectPtr<class UTexture2D> QuestClearedIcon;
};
