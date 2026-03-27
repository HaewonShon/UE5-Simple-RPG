// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Shared/Item/ItemData.h" // FItemStat
#include "ItemManagementSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCachingCompleted)
DECLARE_MULTICAST_DELEGATE(FOnContentChanged)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnhanceCompleted, EEnhanceResult)

UENUM()
enum class EEnhanceResult : uint8
{
	Success,
	Fail,
	Invalid,
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

	FItemDescription PreviewDescription;

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
	void SetEnhanceTargetItem(FItemInstance* Target);
	FItemInstance* GetEnhanceTargetItem() { return EnhanceTarget; }

	bool GetEnhanceData(class APlayerState* PS, FEnhanceDisplayInfo& Out);
	void RequestEnhanceCurrentItem(class APlayerState* PS);

	FOnContentChanged OnEnhanceTargetChanged;
	FOnEnhanceCompleted OnEnhanceCompleted;

protected:
	void BuildCache();
	TMap<FPrimaryAssetId, TObjectPtr<UItemData>> ItemCache;

	bool bCachingCompleted;

	// enhancement info table
	TSoftObjectPtr<UDataTable> EnhanceDataTable;

	FEnhanceTableRow* GetEnhanceData(int32 Level);

	FItemInstance* EnhanceTarget;
};
