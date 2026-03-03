// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/EnumRange.h"
#include "ItemData.generated.h"

struct FStatLine
{
	FText Name;
	FText Value;
};

struct FItemDetail
{
	FText DetailText;
};

UENUM()
enum class ERarity : uint8
{
	None,
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary,
	Relic,
	Count UMETA(Hidden)
};

struct FEquipmentDetail
{
	FText TypeText;
	TArray<TPair<FText, FText>> Stats;
};

using FItemDetailPayload = TVariant<FItemDetail, FEquipmentDetail>;

/**
 *	structure for delivery item description info
 */
struct FItemDescription
{
	FText Name;
	UTexture2D* Icon;
	FText Price;
	ERarity Rarity;
	FItemDetailPayload Payload;
};

/**
 *    Item Data
 */

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
class SIMPLERPG_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual FItemDescription BuildDescriptionData() const;

	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (ReadOnly))
	FPrimaryAssetId AssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

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

/**
 *    Item Instance that used in the game actually
 */

USTRUCT()
struct FItemInstance
{
	GENERATED_BODY()

	FItemInstance();

	FItemInstance(const UItemData* Item, int32 StackCount = 1);

	/* Set item data for Instance */
	bool SetItem(const UItemData* Item, int32 StackCount = 1);

	/* Combine 2 Instances */
	bool AddStack(FItemInstance& OtherInstance);

	bool RemoveStack(int32 Count);

	FPrimaryAssetId ItemID;

	const class UItemData* ItemData;

	int32 StackCount;
};