// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Shared/Item/ItemData.h" // FItemStat
#include "ItemManagementSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCachingCompleted)
DECLARE_MULTICAST_DELEGATE(FOnEnhancementCompleted)

UENUM()
enum class EEnhanceResult : uint8
{
	Success,
	Fail,
};

USTRUCT(BlueprintType)
struct FEnhancementMaterial
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId ItemId;

	UPROPERTY(EditDefaultsOnly)
	int32 Quantity;
};

USTRUCT(BlueprintType)
struct FEnhanceTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	bool bCanEnhance;

	UPROPERTY(EditDefaultsOnly)
	float SuccessRate;

	UPROPERTY(EditDefaultsOnly)
	FItemStat Increase;

	UPROPERTY(EditDefaultsOnly)
	FEnhancementMaterial RequiredMaterials;

	UPROPERTY(EditDefaultsOnly)
	int32 GoldCost;
};

USTRUCT()
struct FEnhanceDisplayInfo
{
	GENERATED_BODY()

	float SuccessRate;
	FItemStat Increase;

	// materials
	int32 OwningGold;
	int32 GoldCost;
};

/**
 *   Item data caching & management(enhance)
 */
UCLASS(Blueprintable)
class SIMPLERPG_API UItemManagementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/*** Item data management ***/
	class UItemData* GetItemData(const FPrimaryAssetId& ID) const;

	FOnCachingCompleted OnCachingCompleted;
	bool IsCachingCompleted() const { return bCachingCompleted; }

	/*** Item enhancement ***/
	bool GetEnhanceData(class APlayerState* PS, FItemInstance& TargetItem, FEnhanceDisplayInfo& Out);
	EEnhanceResult RequestEnhanceItem(class APlayerState* PS, FItemInstance& TargetItem);
	
protected:
	void BuildCache();
	TMap<FPrimaryAssetId, TObjectPtr<UItemData>> ItemCache;

	bool bCachingCompleted;

	// enhancement info table
	TSoftObjectPtr<UDataTable> EnhanceDataTable;
	FEnhanceTableRow* GetEnhanceData(int32 Level);
};
