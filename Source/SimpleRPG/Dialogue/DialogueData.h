// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueData.generated.h"

/**
 *	Data asset for dialogue
 */
UCLASS(BlueprintType)
class SIMPLERPG_API UDialogueData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	FPrimaryAssetId AssetId;

	UPROPERTY(EditDefaultsOnly)
	FText NPCName;

	UPROPERTY(EditDefaultsOnly)
	TArray<FText> Dialogue;
};
