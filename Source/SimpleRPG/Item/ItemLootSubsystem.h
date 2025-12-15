// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ItemData.h"
#include "ItemLootSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemLootSubsystem, Log, All)

/**
 *	Subsystem manages item loot table & determine item to drop, spawn item
 */

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FPrimaryAssetId ItemId;

	UPROPERTY(EditAnywhere)
	float Weight;
};

USTRUCT()
struct FLootInfo
{
	GENERATED_BODY()

	TArray<FLootItem> Item;
	float TotalWeight;
	int32 ItemCount;
};

USTRUCT(BlueprintType)
struct FLootTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EnemyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLootItem> LootItemList;
};

UCLASS(Blueprintable)
class SIMPLERPG_API UItemLootSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UItemLootSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* Item spawn by request from enemy death event */
	void SpawnItem(FGameplayTag EnemyTag, FVector Location);

	/* Item spawn by request from inventory item drop */
	void SpawnItem(const FItemInstance& ItemInstance, FVector Location);

private:
	void ReadLootTable();

	const class UItemData* SelectRandomItem(FGameplayTag EnemyTag);

	TSubclassOf<class AItemActor> ItemActor;

	UPROPERTY()
	TMap<FGameplayTag, FLootInfo> Cache;
};
