// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGAssetManager.h"
#include "Item/ItemData.h"

USimpleRPGAssetManager& USimpleRPGAssetManager::Get()
{
	check(GEngine);
	USimpleRPGAssetManager* Self = Cast<USimpleRPGAssetManager>(GEngine->AssetManager);
	check(Self);
	return *Self;
}

UItemData* USimpleRPGAssetManager::GetItemData(const FPrimaryAssetId& AssetId)
{
	UObject* Asset = GetPrimaryAssetObject(AssetId);

	// Asset이 로드되지 않음
	if (!Asset)
	{
		LoadPrimaryAsset(AssetId);
		Asset = GetPrimaryAssetObject(AssetId);
	}

	return Cast<UItemData>(Asset);
}