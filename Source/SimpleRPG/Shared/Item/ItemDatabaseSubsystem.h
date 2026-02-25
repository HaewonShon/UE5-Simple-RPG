// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemDatabaseSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCachingCompleted)
/**
 *   Subsystem for mapping ID - ItemData 
 */
UCLASS(Blueprintable)
class SIMPLERPG_API UItemDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const class UItemData* Get(const FPrimaryAssetId& ID) const;

	FOnCachingCompleted OnCachingCompleted;
	bool IsCachingCompleted() const { return bCachingCompleted; }
protected:
	void BuildCache();
	TMap<FPrimaryAssetId, class UItemData*> ItemCache;

	bool bCachingCompleted;
};
