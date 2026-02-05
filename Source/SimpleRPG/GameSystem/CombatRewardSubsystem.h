// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "../Item/ItemData.h"
#include "CombatRewardSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCombatRewardSystem, Log, All)

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

	int32 MinGoldAmount;
	int32 MaxGoldAmount;
};

USTRUCT(BlueprintType)
struct FLootTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EnemyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLootItem> LootItemList;

	UPROPERTY(EditAnywhere)
	FIntPoint Gold; // { Min, Max }
};

UCLASS(Blueprintable)
class SIMPLERPG_API UCombatRewardSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UCombatRewardSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* Item spawn by request from enemy death event */
	void SpawnDropFromEnemy(FGameplayTag EnemyTag, FVector Location);

	/* Item spawn by request from inventory item drop */
	void SpawnItem(const FItemInstance& ItemInstance, FVector Location);

private:
	void ReadLootTable();

	const class UItemData* SelectRandomItem(FGameplayTag EnemyTag) const;
	int32 GetRandomGoldAmount(FGameplayTag EnemyTag) const;

	TSubclassOf<class AItemActor> ItemActor;
	TSubclassOf<class AGoldDropActor> GoldDropActor;

	UPROPERTY()
	TMap<FGameplayTag, FLootInfo> EnemyLootInfoCache;

	static constexpr float ItemPickupDelay = 5.f;
};
