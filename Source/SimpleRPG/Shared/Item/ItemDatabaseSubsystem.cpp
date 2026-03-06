// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDatabaseSubsystem.h"
#include "Core/SimpleRPGAssetManager.h"
#include "ItemData.h"

void UItemDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bCachingCompleted = false;
    USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("ItemData", {}, FStreamableDelegate::CreateUObject(this, &UItemDatabaseSubsystem::BuildCache));

    /*if (USimpleRPGAssetManager::Get().AreItemsLoaded())
    {
        BuildCache();
    }
    else
    {
        USimpleRPGAssetManager::Get().OnItemAssetsLoaded.BindUObject(this, &UItemDatabaseSubsystem::BuildCache);
    }*/
}

UItemData* UItemDatabaseSubsystem::Get(const FPrimaryAssetId& ID) const
{
    UE_LOG(LogTemp, Verbose, TEXT("UItemDatabaseSubsystem Item request ID: %s"), *ID.ToString());
    return ItemCache.FindRef(ID);
}

void UItemDatabaseSubsystem::BuildCache()
{
    TArray<FPrimaryAssetId> ItemIds;
    USimpleRPGAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType("ItemData"), ItemIds);

    for (const FPrimaryAssetId& Id : ItemIds)
    {
        UItemData* ItemData = USimpleRPGAssetManager::Get().GetPrimaryAssetObject<UItemData>(Id);
        ensure(ItemData != nullptr);
        ItemCache.Add(Id, ItemData);
    }

    UE_LOG(LogTemp, Log, TEXT("ItemDB Cache built with item count: %i"), ItemCache.Num());
    bCachingCompleted = true;
    OnCachingCompleted.Broadcast();
}
