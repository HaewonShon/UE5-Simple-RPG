// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabaseSubsystem.h"
#include "../SimpleRPGAssetManager.h"
#include "ItemData.h"

void UItemDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("ItemData");

    if (USimpleRPGAssetManager::Get().AreItemsLoaded())
    {
        BuildCache();
    }
    else
    {
        USimpleRPGAssetManager::Get().OnItemAssetsLoaded.BindUObject(this, &UItemDatabaseSubsystem::BuildCache);
    }
}

const UItemData* UItemDatabaseSubsystem::Get(const FPrimaryAssetId& ID) const
{
    UE_LOG(LogTemp, Warning, TEXT("UItemDatabaseSubsystem ID: %s"), *ID.ToString());
    return ItemCache.FindRef(ID);
}

void UItemDatabaseSubsystem::BuildCache()
{
    TArray<FPrimaryAssetId> ItemIds;
    USimpleRPGAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("ItemData"), ItemIds);

    UE_LOG(LogTemp, Warning, TEXT("UItemDatabaseSubsystem buildcache called, res: %i"), ItemIds.Num());
    for (const FPrimaryAssetId& Id : ItemIds)
    {
        UItemData* ItemData = USimpleRPGAssetManager::Get().GetPrimaryAssetObject<UItemData>(Id);

        ItemCache.Add(Id, ItemData);
        UE_LOG(LogTemp, Log, TEXT("UItemDatabaseSubsystem Data added to cache: %s, res: %i"), *Id.ToString(), ItemData != nullptr);
    }
}
