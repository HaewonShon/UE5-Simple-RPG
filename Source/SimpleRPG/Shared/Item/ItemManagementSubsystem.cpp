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
    
    FString TablePath = TEXT("/Game/Interaction/Enhancement/DT_EnhanceData.DT_EnhanceData");
    EnhanceDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));

    if (EnhanceDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemManagementSubsystem: Enhance DataTable Loaded Successfully."));
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

void UItemManagementSubsystem::SetEnhanceTargetItem(FItemInstance* Target)
{
    EnhanceTarget = Target;
    OnEnhanceTargetChanged.Broadcast();
}

bool UItemManagementSubsystem::GetEnhanceData(APlayerState* PS, FEnhanceDisplayInfo& Out)
{
    /*** Validation ***/
    if (!EnhanceTarget)
    {
        return false;
    }

    UItemData* ItemData = EnhanceTarget->DataAsset.Get();
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

    /*** Fill display info ***/
    int32 CurrentItemLevel = EnhanceTarget->EnhancementInfo.EnhancementLevel;
    FEnhanceTableRow* EnhanceData = GetEnhanceData(CurrentItemLevel);
    if (!EnhanceData)
    {
        return false;
    }

    Out.SuccessRate = EnhanceData->SuccessRate;

    FItemInstance PreviewInstance(*EnhanceTarget);
    PreviewInstance.EnhancementInfo.EnhancedStat += EnhanceData->Increase;
    ++PreviewInstance.EnhancementInfo.EnhancementLevel;
    Out.PreviewDescription = PreviewInstance.BuildDescriptionData();

    Out.bCanEnhanceNow = true;
    
    Out.OwningGold = CurrencyComponent->GetCurrencyAmount(ECurrencyType::Gold);
    Out.GoldCost = EnhanceData->GoldCost;
    Out.bHasEnoughGold = (Out.OwningGold >= Out.GoldCost);
    Out.bCanEnhanceNow &= Out.bHasEnoughGold;

    for (const FEnhancementMaterial& RequiredMaterial : EnhanceData->RequiredMaterials)
    {
        FEnhancementRequirementDisplay DisplayInfo;
        DisplayInfo.ItemId = RequiredMaterial.ItemId;
        DisplayInfo.RequiredAmount = RequiredMaterial.Quantity;
        DisplayInfo.OwningAmount = InventoryComponent->RequestItemCount(DisplayInfo.ItemId);
        DisplayInfo.bHasEnoughAmount = (DisplayInfo.RequiredAmount <= DisplayInfo.OwningAmount);
        Out.RequiredMaterials.Add(DisplayInfo);
        Out.bCanEnhanceNow &= DisplayInfo.bHasEnoughAmount;
    }

    return true;
}

void UItemManagementSubsystem::RequestEnhanceCurrentItem(APlayerState* PS)
{
    /*** 1. Validation ***/
    if (!EnhanceTarget 
        || !EnhanceTarget->DataAsset.Get() 
        || !EnhanceTarget->DataAsset->CanEnhance())
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemManagementSubsystem Validation Failed"));
        return;
    }

    UCurrencyComponent* CurrencyComponent = PS->GetComponentByClass<UCurrencyComponent>();
    UInventoryComponent* InventoryComponent = PS->GetComponentByClass<UInventoryComponent>();
    if (!CurrencyComponent || !InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("RequestEnhanceItem: PS is not valid")); 
        return;
    }

    int32 CurrentItemLevel = EnhanceTarget->EnhancementInfo.EnhancementLevel;
    FEnhanceTableRow* EnhanceData = GetEnhanceData(CurrentItemLevel);
    if (!EnhanceData)
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemManagementSubsystem EnhanceData not found"));
        return;
    }

    /*** 2. Requirement check ***/
    if (!CurrencyComponent->TrySpendCurrency(ECurrencyType::Gold, EnhanceData->GoldCost))
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemManagementSubsystem Gold spending Failed"));
        return;
    }

    // 3. Attempt && Result apply
    float RandValue = FMath::FRandRange(0.f, 1.f);
    if (RandValue > EnhanceData->SuccessRate)
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemManagementSubsystem random success Failed"));
        OnEnhanceCompleted.Broadcast(EEnhanceResult::Fail);
    }
    else
    {
        // apply success result to the item
        EnhanceTarget->EnhancementInfo.EnhancedStat += EnhanceData->Increase;
        ++EnhanceTarget->EnhancementInfo.EnhancementLevel;

        UE_LOG(LogTemp, Warning, TEXT("UItemManagementSubsystem Success"));
        OnEnhanceCompleted.Broadcast(EEnhanceResult::Success);
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

    FName RowName = FName(*FString::FromInt(Level));
    return EnhanceDataTable->FindRow<FEnhanceTableRow>(RowName, TEXT("Context_EnhanceLookup"));
}