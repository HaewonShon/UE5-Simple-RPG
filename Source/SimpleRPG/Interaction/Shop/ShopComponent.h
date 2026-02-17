// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/ActionProvider.h"
#include "Shared/Item/ItemData.h"
#include "ShopComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShop, Log, All)

USTRUCT(BlueprintType)
struct FShopItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FPrimaryAssetId ItemId;

	UPROPERTY(EditAnywhere)
	int32 Price;

	UPROPERTY(EditAnywhere)
	int32 Stock;
};

USTRUCT()
struct FShopItem
{
	GENERATED_BODY()
	
	FItemInstance Item;
	int32 Price;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UShopComponent : public UActorComponent, public IActionProvider
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShopComponent();

	/* Action Provider Interface */
	virtual TArray<FActionInfo> CreateAvailableActions(class ASimpleRPGPlayerState* PS);
	virtual FActionInfo CreateContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS);

	void OpenShop(class ASimpleRPGPlayerState* PS);

	bool TryPurchaseItem(FPrimaryAssetId ItemId, int32 Count);
	bool TrySellItem(FPrimaryAssetId ItemId, int32 Count);

	const TArray<struct FItemInstance>& GetShopItems() const;

	FItemDescription GetItemDescription(int32 SlotIndex) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FActionInfo CreateAction(class ASimpleRPGPlayerState* PS);

	void ReadShopDataTable();
	bool CanPurchaseItem(FItemInstance& Item, int32 SellingCount);

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	UDataTable* ShopDataTable;

	UPROPERTY()
	TMap<FPrimaryAssetId, FShopItem> ShopItems;

	UPROPERTY()
	TArray<struct FItemInstance> InstancedShopItems;

	TWeakObjectPtr<class ASimpleRPGPlayerState> PlayerStateRef;
	TWeakObjectPtr<class UItemDatabaseSubsystem> ItemDBSubsystem;
};
