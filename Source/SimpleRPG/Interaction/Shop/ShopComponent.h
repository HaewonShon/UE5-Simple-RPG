// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Core/ActionProvider.h"
#include "Shared/Item/ItemData.h"
#include "ShopComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShop, Log, All)

DECLARE_DELEGATE(FOnShopContentChanged);

UENUM()
enum class EPurchaseResult : uint8
{
	Success,
	Failed_NotValid,
	Failed_NotEnoughItemAmount,
	Failed_NotEnoughSpace,
	Failed_NotEnoughCurrency,
};

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
	void SetPlayerStateRef(class ASimpleRPGPlayerState* PS);

	/* Action Provider Interface */
	virtual TArray<FActionInfo> CreateAvailableActions(class ASimpleRPGPlayerState* PS);
	virtual FActionInfo CreateContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS);

	void RequestPurchaseItem(int32 SlotIndex);
	void ProcessPurchaseResult(EPurchaseResult Result);

	EPurchaseResult TryPurchaseItem(int32 SlotIndex, int32 Count);
	bool TrySellItem(FPrimaryAssetId ItemId, int32 Count);

	void CloseShop();
	const TArray<FShopItem>& GetShopItemList() const;

	FItemDescription GetItemDescription(int32 SlotIndex) const;

	FOnShopContentChanged OnShopContentChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ReadShopDataTable();
	EPurchaseResult CanPurchaseItem(FItemInstance& Item, int32 SellingCount);

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	UDataTable* ShopDataTable;

	UPROPERTY()
	TArray<FShopItem> ShopItemList;

	TWeakObjectPtr<class ASimpleRPGPlayerState> PlayerStateRef;
	TWeakObjectPtr<class UItemDatabaseSubsystem> ItemDBSubsystem;
};
