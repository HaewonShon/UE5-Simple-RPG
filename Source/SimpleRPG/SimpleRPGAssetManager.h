// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SimpleRPGAssetManager.generated.h"

/**
 *	AssetManager class for global data management(item)
 */

UCLASS(BlueprintType)
class SIMPLERPG_API USimpleRPGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static USimpleRPGAssetManager& Get();

	class UItemData* GetItemData(const FPrimaryAssetId& AssetId);
};