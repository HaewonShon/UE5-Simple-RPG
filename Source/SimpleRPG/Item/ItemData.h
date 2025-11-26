// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/EnumRange.h"
#include "ItemData.generated.h"

/**
 *    Item Data
 */

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Weapon,
	Armor,
	Consumable,
	Material,
	Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EItemCategory, EItemCategory::Count); // Register Enum Range using Count

UCLASS(BlueprintType)
class SIMPLERPG_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	bool bIsStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize;

	// TSubclassOf<class UGameplayEffect> StatEffect; - dynamicalliy generated
};

/**
 *    Item Instance that used in the game actually
 */

USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	FItemInstance();

	FItemInstance(UItemData* Item, int32 StackCount = 1);

	/* Set item data for Instance */
	bool SetItem(UItemData* Item, int32 StackCount = 1);

	/* Combine 2 Instances */
	bool AddStack(FItemInstance& OtherInstance);

	FPrimaryAssetId ItemID;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UItemData> ItemData;

	int32 StackCount;
};