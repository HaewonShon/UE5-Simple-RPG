// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SimpleRPGAssetManager.generated.h"

/**
 *	AssetManager class for global data management(item)
 */

DECLARE_DELEGATE(FOnAssetLoaded);

UCLASS(BlueprintType)
class SIMPLERPG_API USimpleRPGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static USimpleRPGAssetManager& Get();

	virtual void StartInitialLoading() override;

	bool AreItemsLoaded() const { return bAreItemsLoaded; }
	FOnAssetLoaded OnItemAssetsLoaded;

protected:
	void OnItemsLoaded();
	bool bAreItemsLoaded;
};