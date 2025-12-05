// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabaseSubsystem.h"
#include "../SimpleRPGAssetManager.h"
#include "ItemData.h"

void UItemDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

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

    USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("ItemData");
    for (const FPrimaryAssetId& Id : ItemIds)
    {
        UE_LOG(LogTemp, Warning, TEXT("Id.Type = %s, Id.Name = %s"),
            *Id.PrimaryAssetType.ToString(),
            *Id.PrimaryAssetName.ToString());

        UItemData* AssetObj = USimpleRPGAssetManager::Get().GetPrimaryAssetObject<UItemData>(Id);
        //UItemData* ItemData = Cast<UItemData>(AssetObj);

        //ItemCache.Add(Id, ItemData);
        UE_LOG(LogTemp, Log, TEXT("UItemDatabaseSubsystem Data added to cache: %s, res: %i"), *Id.ToString(), AssetObj != nullptr);
    }
}
