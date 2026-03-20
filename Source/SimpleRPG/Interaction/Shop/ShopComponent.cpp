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
	UE_LOG(LogShop, Warning, TEXT("RequestPurchaseItem called"), SlotIndex);

	if (SlotIndex < 0 || SlotIndex >= ShopItemList.Num())
	{
		UE_LOG(LogShop, Warning, TEXT("RequestPurchaseItem: Given SlotIndex is not valid - SlotIndex = %i"), SlotIndex);
		return;
	}

	FShopItem& ShopItem = ShopItemList[SlotIndex];
	if (ShopItem.Item.DataAsset->bIsStackable)
	{
		check(PlayerStateRef.IsValid());

		// display quantity window;
		UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: quantity check start"));
		if (UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem())
		{
			UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: UISubsystem found"));
			UQuantityConfirmationWidget* Widget = UISubsystem->RequestCreateQuantityWidget();
			if (Widget)
			{
				Widget->OnQuantityConfirmed.BindLambda([this, SlotIndex](int32 Count) {
					ETransactionResult Result = this->TryPurchaseItem(SlotIndex, Count);
					ProcessPurchaseResult(Result);
					});
				UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: quantity widget displayed"));
			}
			else
			{
				UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: Failed to display widget"));
			}
		}
		UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: quantity check end"));
	}
	else
	{
		ETransactionResult Result = TryPurchaseItem(SlotIndex, 1);
		ProcessPurchaseResult(Result);
	}
	UE_LOG(LogShop, Error, TEXT("RequestPurchaseItem: end"));
}

void UShopComponent::RequestSellItem(int32 InventorySlotIndex)
{
	UE_LOG(LogShop, Warning, TEXT("RequestSellItem called"));

	const FItemInstance& ItemInstance = PlayerStateRef->GetInventoryComponent()->GetPage().GetItemInstance(InventorySlotIndex);
	if (!ItemInstance.IsValid())
	{
		UE_LOG(LogShop, Warning, TEXT("RequestSellItem: Given ItemInstance is not valid"));
		return;
	}

	UItemData* ItemData = ItemInstance.DataAsset.Get();
	if (ItemData->bIsStackable)
	{
		check(PlayerStateRef.IsValid());

		// display quantity window;
		if (UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem())
		{
			UQuantityConfirmationWidget* Widget = UISubsystem->RequestCreateQuantityWidget();
			if (Widget)
			{
				Widget->OnQuantityConfirmed.BindLambda([this, InventorySlotIndex](int32 Count) {
					ETransactionResult Result = this->TrySellItem(InventorySlotIndex, Count);
					ProcessSellResult(Result);
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
		ETransactionResult Result = TrySellItem(InventorySlotIndex, 1);
		ProcessSellResult(Result);
	}
}

void UShopComponent::ProcessPurchaseResult(ETransactionResult Result)
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessPurchaseResult called"));
	UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem();
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessPurchaseResult: Cannot get UI Subsystem"));
		return;
	}

	switch (Result)
	{
	case ETransactionResult::Success:
	{
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Item purchased!")));
		OnShopContentChanged.Broadcast();
	}
	break;
	case ETransactionResult::Failed_NotValid:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not valid request.")));
		break;
	case ETransactionResult::Failed_NotEnoughCurrency:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough money.")));
		break;
	case ETransactionResult::Failed_NotEnoughItemAmount:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough item amount.")));
		break;
	case ETransactionResult::Failed_NotEnoughSpace:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough space in inventory")));
		break;
	}
}

ETransactionResult UShopComponent::TryPurchaseItem(int32 SlotIndex, int32 Amount)
{
	if (SlotIndex < 0 || SlotIndex >= ShopItemList.Num() || Amount < 1)
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: Given Index is not valid, index: %i"), SlotIndex);
		return ETransactionResult::Failed_NotValid;
	}

	FShopItem& ShopItem = ShopItemList[SlotIndex];
	if (!ShopItem.Item.DataAsset.IsValid())
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: ShopItem in Given Index is not valid, index: %i"), SlotIndex);
		return ETransactionResult::Failed_NotValid;
	}

	if (Amount > ShopItem.Item.StackCount)
	{
		UE_LOG(LogShop, Warning, TEXT("TryPurchaseItem: requested Item count is not valid, item stack count: %i, requested Amount: %i"), ShopItem.Item.StackCount, Amount);
		return ETransactionResult::Failed_NotEnoughItemAmount;
	}

	int32 RequiredCost = ShopItem.Price * Amount;

	FItemInstance ItemToBuy;
	ItemToBuy.SetItem(ShopItem.Item.DataAsset.Get(), Amount);

	ETransactionResult CanPurchaseResult = CanPurchaseItem(ItemToBuy, RequiredCost);
	if (CanPurchaseResult != ETransactionResult::Success)
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
		return ETransactionResult::Failed_NotEnoughSpace;
	}

	// TODO : UI Notify Success
	ShopItem.Item.StackCount -= Amount;
	return ETransactionResult::Success;
}

void UShopComponent::ProcessSellResult(ETransactionResult Result)
{
	UE_LOG(LogTemp, Warning, TEXT("ProcessSellResult called"));
	UUISubsystem* UISubsystem = PlayerStateRef->GetUISubsystem();
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessPurchaseResult: Cannot get UI Subsystem"));
		return;
	}

	switch (Result)
	{
	case ETransactionResult::Success:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Item sold!")));
		break;
	case ETransactionResult::Failed_NotValid:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not valid request.")));
		break;
	case ETransactionResult::Failed_NotEnoughItemAmount:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Not enough item to sell.")));
		break;
	case ETransactionResult::Failed_NotEnoughSpace:
		UISubsystem->RequestDisplayMessageBox(FText::FromString(TEXT("Failed to grant gold.")));
		break;
	}
}

ETransactionResult UShopComponent::TrySellItem(int32 InventorySlotIndex, int32 Count)
{
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	const FItemInstance& ItemInstance = InventoryComponent->GetPage().GetItemInstance(InventorySlotIndex);
	const UItemData* ItemData = ItemInstance.DataAsset.Get();
	if (!ItemData || Count < 1)
	{
		return ETransactionResult::Failed_NotValid;
	}

	ETransactionResult CanPurchaseResult = CanSellItem(ItemInstance, Count);
	if (CanPurchaseResult != ETransactionResult::Success)
	{
		return CanPurchaseResult;
	}

	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();

	if (!CurrencyComponent->TryAddCurrency(ECurrencyType::Gold, Count * ItemData->SellPrice))
	{
		return ETransactionResult::Failed_NotEnoughSpace;
	}

	if (!InventoryComponent->RemoveItem(InventorySlotIndex, Count))
	{
		CurrencyComponent->TrySpendCurrency(ECurrencyType::Gold, Count * ItemData->SellPrice); // rollback currency
		return ETransactionResult::Failed_NotEnoughItemAmount;
	}

	return ETransactionResult::Success;
}

ETransactionResult UShopComponent::CanSellItem(const FItemInstance& Item, int32 SellingCount)
{
	if (Item.StackCount == 0)
	{
		return ETransactionResult::Failed_NotValid;
	}

	if (Item.StackCount < SellingCount)
	{
		return ETransactionResult::Failed_NotEnoughItemAmount;
	}

	return ETransactionResult::Success;
}

void UShopComponent::CloseShop()
{
	ASimpleRPGPlayerController* PC = Cast<ASimpleRPGPlayerController>(PlayerStateRef->GetPlayerController());
	if (PC)
	{
		PC->CloseShop();
	}
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
		if (Item.DataAsset.IsValid())
		{
			FItemDescription Description = Item.DataAsset->BuildDescriptionData();
			Description.Price = FText::Format(FText::FromString(TEXT("Buy price: {0}G")), ShopItemList[SlotIndex].Price);
			return Description;
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
		UItemData* Item = ItemDBSubsystem->Get(Row.ItemId);
		if (!Item)
		{
			UE_LOG(LogShop, Warning, TEXT("Item ID not valid: %s"), *Row.ItemId.ToString());
		}
		else
		{
			ShopItemList.Emplace(FItemInstance(Item, Row.Stock), Row.Price);
		}
		});

	UE_LOG(LogShop, Log, TEXT("%s's shop items initialized with total count: %i"), *GetOwner()->GetName(), ShopItemList.Num());
}

ETransactionResult UShopComponent::CanPurchaseItem(FItemInstance& Item, int32 Cost)
{
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	// Check if player has enough currency to buy
	if (!CurrencyComponent->CanSpendCurrency(ECurrencyType::Gold, Cost))
	{
		// TODO : UI Notify Failed - not enough gold
		return ETransactionResult::Failed_NotEnoughCurrency;
	}

	if (!InventoryComponent->CanAddItem(Item))
	{
		// TODO : UI Notify Failed - not enough space
		return ETransactionResult::Failed_NotEnoughSpace;
	}

	return ETransactionResult::Success;
}