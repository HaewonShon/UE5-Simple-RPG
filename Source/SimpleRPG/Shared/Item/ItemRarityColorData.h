// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.h"
#include "ItemRarityColorData.generated.h"

/**
 *		Color data for displaying item rarity in description widget / slot background
 */
UCLASS()
class SIMPLERPG_API UItemRarityColorData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FLinearColor GetColorForRarity(ERarity Rarity) const { return Colors.FindRef(Rarity); }

	UPROPERTY(EditDefaultsOnly, Category = "Rarity")
	TMap<ERarity, FLinearColor> Colors;
};
