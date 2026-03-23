// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManagementSubsystem.h"
#include "Core/SimpleRPGAssetManager.h"
#include "ItemData.h"

#include "Player/SimpleRPGPlayerState.h"
#include "Player/Components/CurrencyComponent.h"
#include "Player/Components/InventoryComponent.h"

void UItemManagementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    bCachingCompleted = false;
    USimpleRPGAssetManager::Get().LoadPrimaryAssetsWithType("ItemData", {}, FStreamableDelegate::CreateUObject(this, &UItemManagementSubsystem::BuildCache));
    
    FString TablePath = TEXT("/Game/Data/DT_Enhance.DT_Enhance");
    EnhanceDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));

    if (EnhanceDataTable)
    {
        UE_LOG(LogTemp, Log, TEXT("ItemManagementSubsystem: Enhance DataTable Loaded Successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ItemManagementSubsystem: Failed to load Enhance DataTable at %s"), *TablePath);
    }
    /*if (USimpleRPGAssetManager::Get().AreItemsLoaded())
    {
        BuildCache();
    }
    else
    {
        USimpleRPGAssetManager::Get().OnItemAssetsLoaded.BindUObject(this, &UItemDatabaseSubsystem::BuildCache);
    }*/
}

UItemData* UItemManagementSubsystem::GetItemData(const FPrimaryAssetId& ID) const
{
    UE_LOG(LogTemp, Verbose, TEXT("UItemDatabaseSubsystem Item request ID: %s"), *ID.ToString());
    return ItemCache.FindRef(ID);
}

bool UItemManagementSubsystem::GetEnhanceData(APlayerState* PS, FItemInstance& TargetItem, FEnhanceDisplayInfo& Out)
{
    UItemData* ItemData = TargetItem.DataAsset.Get();
    if (!(ItemData && ItemData->CanEnhance()))
    {
        return false;
    }

    UCurrencyComponent* CurrencyComponent = PS->GetComponentByClass<UCurrencyComponent>();
    UInventoryComponent* InventoryComponent = PS->GetComponentByClass<UInventoryComponent>();
    if (!CurrencyComponent || !InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("GetEnhanceData: PS is not valid"));
        return false;
    }
    // read table
    int32 CurrentItemLevel = TargetItem.EnhancementInfo.EnhancementLevel;
    FEnhanceTableRow* EnhanceData = GetEnhanceData(CurrentItemLevel);
    if (!EnhanceData)
    {
        return false;
    }

    Out.SuccessRate = EnhanceData->SuccessRate;
    Out.Increase = EnhanceData->Increase;
    Out.OwningGold = CurrencyComponent->GetCurrencyAmount(ECurrencyType::Gold);
    Out.GoldCost = EnhanceData->GoldCost;

    // TODO : Read/fill required material quantity

    return false;
}

EEnhanceResult UItemManagementSubsystem::RequestEnhanceItem(APlayerState* PS, FItemInstance& TargetItem)
{
    // 1. Validation
    UItemData* ItemData = TargetItem.DataAsset.Get();
    if (!(ItemData && ItemData->CanEnhance()))
    {
        return EEnhanceResult::Fail;
    }

    UCurrencyComponent* CurrencyComponent = PS->GetComponentByClass<UCurrencyComponent>();
    UInventoryComponent* InventoryComponent = PS->GetComponentByClass<UInventoryComponent>();
    if (!CurrencyComponent || !InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("RequestEnhanceItem: PS is not valid"));
        return EEnhanceResult::Fail;
    }

    // 2. Check requirements
    int32 CurrentItemLevel = TargetItem.EnhancementInfo.EnhancementLevel;
    FEnhanceTableRow* EnhanceData = GetEnhanceData(CurrentItemLevel);
    if (!EnhanceData)
    {
        return EEnhanceResult::Fail;
    }

    if (!CurrencyComponent->TrySpendCurrency(ECurrencyType::Gold, EnhanceData->GoldCost))
    {
        return EEnhanceResult::Fail;
    }

    // 3. Attempt && Result apply
    float RandValue = FMath::FRandRange(0.f, 1.f);
    if (RandValue > EnhanceData->SuccessRate)
    {
        return EEnhanceResult::Fail;
    }
    else
    {
        // apply success result to the item
        TargetItem.EnhancementInfo.EnhancedStat += EnhanceData->Increase;
        ++TargetItem.EnhancementInfo.EnhancementLevel;

        return EEnhanceResult::Success;
    }
}

void UItemManagementSubsystem::BuildCache()
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

FEnhanceTableRow* UItemManagementSubsystem::GetEnhanceData(int32 Level)
{
    if (!EnhanceDataTable)
    {
        return nullptr;
    }

    // 레벨 번호를 RowName으로 사용 (예: "1", "2"...)
    FName RowName = FName(*FString::FromInt(Level));

    // 테이블에서 행 찾기
    return EnhanceDataTable->FindRow<FEnhanceTableRow>(RowName, TEXT("Context_EnhanceLookup"));
}
