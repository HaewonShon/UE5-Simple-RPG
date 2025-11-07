// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	ITEM_CATEGORY_COUNT
};

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

USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	void SetItem(UItemData* Item);

	FPrimaryAssetId ItemID;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UItemData> ItemData;

	int32 StackCount;
};