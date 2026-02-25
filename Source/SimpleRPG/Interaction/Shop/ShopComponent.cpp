// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopComponent.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Player/SimpleRPGPlayerController.h"
#include "Player/Components/CurrencyComponent.h"
#include "Player/Components/InventoryComponent.h"
#include "Shared/UI/UISubsystem.h"
#include "Shared/Item/UI/QuantityConfirmationWidget.h"

DEFINE_LOG_CATEGORY(LogShop)

// Sets default values for this component's properties
UShopComponent::UShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UShopComponent::SetPlayerStateRef(ASimpleRPGPlayerState* PS)
{
	PlayerStateRef = PS;
}

TArray<FActionInfo> UShopComponent::CreateAvailableActions(ASimpleRPGPlayerState* PS)
{
	TArray<FActionInfo> Actions;

	FActionInfo Action;
	Action.ContextOwner = this;
	Action.DisplayName = FText::FromString("Shop");
	Action.Icon = nullptr;
	Action.Type = EActionType::Shop;

	ASimpleRPGPlayerController* PC = Cast<ASimpleRPGPlayerController>(PS->GetPlayerController());
	Action.OnActionExecuted.BindUObject(PC, &ASimpleRPGPlayerController::OpenShop, this);

	Actions.Add(Action);
	return Actions;
}

FActionInfo UShopComponent::CreateContextAction(FGameplayTag ActionTag, ASimpleRPGPlayerState* PS)
{
	return FActionInfo();
}

void UShopComponent::RequestPurchaseItem(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= ShopItemList.Num())
	{
		UE_LOG(LogShop, Warning, TEXT("RequestPurchaseItem: Given SlotIndex is not valid - SlotIndex = %i"), SlotIndex);
		return;
	}

	FShopItem& ShopItem = ShopItemList[SlotIndex];
	if (ShopItem.Item.ItemData->bIsStackable)
	{
		check(PlayerStateRef.IsValid());

		// display quantity window;
		if (UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem())
		{
			UQuantityConfirmationWidget* Widget = UISubsystem->RequestCreateQuantityWidget();
			if (Widget)
			{
				Widget->OnQuantityConfirmed.BindLambda([this, SlotIndex](int32 Count) {
					EPurchaseResult Result = this->TryPurchaseItem(SlotIndex, Count);
					ProcessPurchaseResult(Result);
					});
			}
			else
			{
				UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: Failed to display widget"));
			}
		}
	}
	else
	{
		EPurchaseResult Result = TryPurchaseItem(SlotIndex, 1);
		ProcessPurchaseResult(Result);
	}
}

void UShopComponent::ProcessPurchaseResult(EPurchaseResult Result)
{
	UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem();
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessPurchaseResult: Cannot get UI Subsystem"));
		return;
	}

	switch (Result)
	{
	case EPurchaseResult::Success:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Item purchased!")));
		break;
	case EPurchaseResult::Failed_NotEnoughCurrency:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough money.")));
		break;
	case EPurchaseResult::Failed_NotEnoughItemAmount:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough item amount.")));
		break;
	case EPurchaseResult::Failed_NotEnoughSpace:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough space in inventory")));
		break;
	}
}

EPurchaseResult UShopComponent::TryPurchaseItem(int32 SlotIndex, int32 Amount)
{
	if (SlotIndex < 0 || SlotIndex >= ShopItemList.Num())
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: Given Index is not valid, index: %i"), SlotIndex);
		return EPurchaseResult::Failed_NotValid;
	}

	FShopItem& ShopItem = ShopItemList[SlotIndex];
	if (!ShopItem.Item.ItemData)
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: ShopItem in Given Index is not valid, index: %i"), SlotIndex);
		return EPurchaseResult::Failed_NotValid;
	}

	if (Amount > ShopItem.Item.StackCount)
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: requested Item count is not valid, item stack count: %i, requested Amount: %i"), ShopItem.Item.StackCount, Amount);
		return EPurchaseResult::Failed_NotEnoughItemAmount;
	}

	int32 RequiredCost = ShopItem.Price * Amount;

	FItemInstance ItemToBuy;
	ItemToBuy.SetItem(ShopItem.Item.ItemData, Amount);

	EPurchaseResult CanPurchaseResult = CanPurchaseItem(ItemToBuy, RequiredCost);
	if (CanPurchaseResult != EPurchaseResult::Success)
	{
		return CanPurchaseResult;
	}

	// Add Item to player
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	CurrencyComponent->TrySpendCurrency(ECurrencyType::Gold, RequiredCost);
	if (!InventoryComponent->AddItem(ItemToBuy))
	{
		// rollback if failed to add item
		CurrencyComponent->TryAddCurrency(ECurrencyType::Gold, RequiredCost);
		return EPurchaseResult::Failed_NotEnoughSpace;
	}

	// TODO : UI Notify Success
	ShopItem.Item.StackCount -= Amount;
	return EPurchaseResult::Success;
}

bool UShopComponent::TrySellItem(FPrimaryAssetId ItemId, int32 SellingCount)
{
	const UItemData* ItemData = ItemDBSubsystem->Get(ItemId);
	if (!ItemData)
	{
		UE_LOG(LogShop, Warning, TEXT("Given Item Id is not valid, ID: %s"), *ItemId.ToString());
		return false;
	}

	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();

	int32 ItemCountInInventory = InventoryComponent->RequestItemCount(ItemId);
	int32 TotalItemPrice = ItemData->SellPrice;

	if (ItemCountInInventory < SellingCount)
	{
		// UI NOTIFY failed - not enough count
		return false;
	}

	if (CurrencyComponent->CanAddCurrency(ECurrencyType::Gold, TotalItemPrice))
	{
		// UI Notify failed - Cannot grant gold
		return false;
	}



	return true;
}

const TArray<FShopItem>& UShopComponent::GetShopItemList() const
{
	return ShopItemList;
}

FItemDescription UShopComponent::GetItemDescription(int32 SlotIndex) const
{
	if (SlotIndex >= 0 && SlotIndex < ShopItemList.Num())
	{
		const FItemInstance& Item = ShopItemList[SlotIndex].Item;
		if (Item.ItemData)
		{
			return Item.ItemData->BuildDescriptionData();
		}
	}	
	return FItemDescription();
}

// Called when the game starts
void UShopComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ShopDataTable)
	{
		UE_LOG(LogShop, Warning, TEXT("%s Shop does not have data table"), *GetOwner()->GetName());
	}
	else
	{
		ItemDBSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemDatabaseSubsystem>();
		check(ItemDBSubsystem.IsValid());

		if (ItemDBSubsystem->IsCachingCompleted())
		{
			ReadShopDataTable();
		}
		else
		{
			ItemDBSubsystem->OnCachingCompleted.AddUObject(this, &UShopComponent::ReadShopDataTable);
		}
	}
}

void UShopComponent::ReadShopDataTable()
{
	ShopDataTable->ForeachRow<FShopItemRow>(TEXT("Reading shop data table"), [this](const FName& RowName, const FShopItemRow& Row) {
		const UItemData* Item = ItemDBSubsystem->Get(Row.ItemId);
		if (!Item)
		{
			UE_LOG(LogShop, Warning, TEXT("Item ID not valid: %s"), *Row.ItemId.ToString());
		}
		else
		{
			ShopItemList.Emplace(FItemInstance(ItemDBSubsystem->Get(Row.ItemId), Row.Stock), Row.Price);
		}
		});

	UE_LOG(LogShop, Log, TEXT("%s's shop items initialized with total count: %i"), *GetOwner()->GetName(), ShopItemList.Num());
}

EPurchaseResult UShopComponent::CanPurchaseItem(FItemInstance& Item, int32 Cost)
{
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	// Check if player has enough currency to buy
	if (!CurrencyComponent->CanSpendCurrency(ECurrencyType::Gold, Cost))
	{
		// TODO : UI Notify Failed - not enough gold
		return EPurchaseResult::Failed_NotEnoughCurrency;
	}

	if (!InventoryComponent->CanAddItem(Item))
	{
		// TODO : UI Notify Failed - not enough space
		return EPurchaseResult::Failed_NotEnoughSpace;
	}

	return EPurchaseResult::Success;
}