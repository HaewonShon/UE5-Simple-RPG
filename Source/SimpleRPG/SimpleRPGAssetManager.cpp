// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGAssetManager.h"
#include "Item/ItemData.h"

USimpleRPGAssetManager& USimpleRPGAssetManager::Get()
{
	check(GEngine);
	USimpleRPGAssetManager* Self = CastChecked<USimpleRPGAssetManager>(GEngine->AssetManager);
	return *Self;
}

void USimpleRPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	bAreItemsLoaded = false;
	UE_LOG(LogTemp, Log, TEXT("AssetManager InitialLoading"));
	
	// Item Load
	LoadPrimaryAssetsWithType(FPrimaryAssetType("ItemData"), {},
		FStreamableDelegate::CreateUObject(this, &USimpleRPGAssetManager::OnItemsLoaded)
	);
}

void USimpleRPGAssetManager::OnItemsLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("AssetManager ItemData Loaded"));
	bAreItemsLoaded = true;
	if (OnItemAssetsLoaded.IsBound())
	{
		OnItemAssetsLoaded.Execute();
	}
}