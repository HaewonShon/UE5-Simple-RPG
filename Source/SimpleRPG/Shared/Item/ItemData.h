// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Common/ActionableAsset.h"
#include "Shared/Common/SlotTypes.h"
#include "Shared/Item/ItemTypes.h"
#include "ItemData.generated.h"


UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Equipment,
	Consumable,
	Material,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EItemCategory, EItemCategory::Count); // Register Enum Range using Count

UCLASS(BlueprintType, meta = (DisplayName = "Item Data Asset"))
class SIMPLERPG_API UItemData : public UActionableAsset
{
	GENERATED_BODY()

public:
	UItemData() { MaxStackSize = 1; }

	virtual bool CanExecute(AActor* Executer, const FSlotAddress& Address) const override;
	virtual void Execute(AActor* Executer, const FSlotAddress& Address) override;

	virtual bool CanEnhance() const { return false; }
	virtual FItemStat GetBaseStat() const { return FItemStat(); }

	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual FItemDescription BuildDescriptionData() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	bool bIsStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (EditCondition = "bIsStackable"))
	int32 MaxStackSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText DescriptionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	ERarity Rarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Shop")
	int32 SellPrice;
};

USTRUCT()
struct FEnhancementInfo
{
	GENERATED_BODY()

	FEnhancementInfo() : EnhancementLevel(0) {}

	int32 EnhancementLevel;
	FItemStat EnhancedStat;
};

/**
 *    Item Instance that used in the game actually
 */

USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	FItemInstance();
	FItemInstance(const FItemInstance& Other);
	FItemInstance(UItemData* Item, int32 StackCount = 1);

	/* Set item data for Instance */
	bool SetItem(UItemData* Item, int32 StackCount = 1);
	bool SplitFrom(FItemInstance& Other, int32 StackCount = 1);

	/* Combine 2 Instances */
	bool AddStack(FItemInstance& OtherInstance);
	bool RemoveStack(int32 Count);

	bool IsValid() const { return DataAsset.IsValid(); }	
	FItemStat GetTotalStat() const { return DataAsset->GetBaseStat() + EnhancementInfo.EnhancedStat; }
	FItemDescription BuildDescriptionData() const;

	FPrimaryAssetId ItemID;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UItemData> DataAsset;

	UPROPERTY(EditAnywhere)
	int32 Amount;

	UPROPERTY()
	FEnhancementInfo EnhancementInfo;
};
