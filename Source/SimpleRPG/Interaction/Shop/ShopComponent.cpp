// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopComponent.h"
#include "Shared/Item/ItemDatabaseSubsystem.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Player/SimpleRPGPlayerController.h"
#include "Player/Components/CurrencyComponent.h"
#include "Player/Components/InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogShop)

// Sets default values for this component's properties
UShopComponent::UShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
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

void UShopComponent::OpenShop(ASimpleRPGPlayerState* PS)
{
	PlayerStateRef = PS;

	if (ASimpleRPGPlayerController* PC = Cast<ASimpleRPGPlayerController>(PS->GetPlayerController()))
	{
		// open shop ui
		PC->OnShopOpenRequested.ExecuteIfBound(this);
	}
	else
	{
		UE_LOG(LogShop, Warning, TEXT("Controller of PlayerState is not Valid"));
	}

}

bool UShopComponent::TryPurchaseItem(FPrimaryAssetId ItemId, int32 Count)
{
	FShopItem* ShopItem = ShopItems.Find(ItemId);
	if (!ShopItem)
	{
		UE_LOG(LogShop, Warning, TEXT("Given Item Id is not valid, ID: %s"), *ItemId.ToString());
		return false;
	}
	int32 RequiredCost = ShopItem->Price * Count;

	FItemInstance ItemToBuy;
	ItemToBuy.SetItem(ShopItem->Item.ItemData, RequiredCost);

	if (!CanPurchaseItem(ItemToBuy, RequiredCost))
	{
		return false;
	}

	// Add Item to player
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	CurrencyComponent->TrySpendCurrency(ECurrencyType::Gold, RequiredCost);
	if (!InventoryComponent->AddItem(ItemToBuy))
	{
		// rollback if failed to add item
		CurrencyComponent->TryAddCurrency(ECurrencyType::Gold, RequiredCost);
		return false;
	}

	// TODO : UI Notify Success
	return true;
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

const TMap<FPrimaryAssetId, FShopItem>& UShopComponent::GetShopItems() const
{
	return ShopItems;
}

FItemDescription UShopComponent::GetItemDescription(int32 SlotIndex) const
{
	if (SlotIndex >= 0 && SlotIndex < InstancedShopItems.Num())
	{
		const FItemInstance& Item = InstancedShopItems[SlotIndex];
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

		ReadShopDataTable();
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
			ShopItems.Emplace(Row.ItemId, { FItemInstance(ItemDBSubsystem->Get(Row.ItemId), Row.Stock), Row.Price });
		}
		});

	UE_LOG(LogShop, Log, TEXT("%s's shop items initialized with total count: %i"), *GetOwner()->GetName(), ShopItems.Num());
}

bool UShopComponent::CanPurchaseItem(FItemInstance& Item, int32 Cost)
{
	UCurrencyComponent* CurrencyComponent = PlayerStateRef->GetComponentByClass<UCurrencyComponent>();
	UInventoryComponent* InventoryComponent = PlayerStateRef->GetComponentByClass<UInventoryComponent>();

	// Check if player has enough currency to buy
	if (!CurrencyComponent->CanSpendCurrency(ECurrencyType::Gold, Cost))
	{
		// TODO : UI Notify Failed - not enough gold
		return false;
	}

	if (!InventoryComponent->CanAddItem(Item))
	{
		// TODO : UI Notify Failed - not enough space
		return false;
	}

	return true;
}

