// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Shop/UI/ShopWidget.h"
#include "Interaction/Shop/UI/ShopSlotWidget.h"
#include "../ShopComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"

#include "Shared/Item/UI/ItemSlotDragWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"
#include "Shared/Item/UI/ItemGridWidget.h"
#include "Player/UI/Inventory/InventoryWidget.h"


void UShopWidget::NativeConstruct()
{
	// Slot Setup
	if (GridWidget && SlotWidgetClass.Get())
	{
		GridWidget->SetSlotType(ESlotType::Shop);
		GridWidget->OnHovered.AddUObject(this, &UShopWidget::OnSlotHovered);
		GridWidget->OnDrop.AddUObject(this, &UShopWidget::OnSlotDropped);
		GridWidget->OnHoverEnded.AddUObject(this, &UShopWidget::OnSlotHoverEnded);
	}
	else
	{
		///UE_LOG(LogShop, Warning, TEXT("Failed to create Item Slots, %i, %i"), SlotGridPanel == nullptr, SlotWidgetClass == nullptr);
	}

	CloseButton->OnClicked.AddDynamic(this, &UShopWidget::OnCloseButtonClicked);
}

void UShopWidget::InitializeShop(UShopComponent* ShopComponent)
{
	if (!ShopComponent)
	{
		UE_LOG(LogShop, Warning, TEXT("ShopWidget Initialize: Given shop component is not valid"));
		return;
	}

	ShopComponentRef = ShopComponent;
	ShopComponentRef->OnShopContentChanged.AddUObject(this, &UShopWidget::UpdateShopContents);
	UpdateShopContents();
}

void UShopWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidget)
{
	ItemDescriptionWidgetRef = DescriptionWidget;
	InventoryWidget->SetDescriptionWidgetRef(DescriptionWidget);
}

void UShopWidget::UpdateShopContents()
{
	const TArray<FShopItem>& ShopItemList = ShopComponentRef->GetShopItemList();
	for(int32 Index = 0; Index < ShopItemList.Num(); ++Index)
	{
		const FShopItem& ShopItem = ShopItemList[Index];
		if (ShopItem.Item.DataAsset.IsValid())
		{
			FSlotContent Content;
			Content.ContentAsset = ShopItem.Item.DataAsset;
			Content.Quantity = ShopItem.Item.StackCount;
			Content.SlotAddress = FSlotAddress(ESlotType::Shop, Index);

			UShopSlotWidget* SlotWidget = Cast<UShopSlotWidget>(GridWidget->GetSlotAt(Index));
			SlotWidget->UpdateSlot(Content);
			SlotWidget->SetPrice(ShopItem.Price);
		}
	}
}

void UShopWidget::OnSlotHovered(const FSlotAddress& SlotAddress)
{
	const TArray<FShopItem>& ShopItemList = ShopComponentRef->GetShopItemList();
	FItemInstance SlotItem = ShopItemList[SlotAddress.SlotIndex].Item;
	if (!SlotItem.IsValid())
	{
		return;
	}

	FItemDescription Description;
	Description = ShopComponentRef->GetItemDescription(SlotAddress.SlotIndex);

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInScreen(MousePos);
}

void UShopWidget::OnSlotHoverEnded()
{
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotHoverEnded Called"));
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UShopWidget::OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotDropped Called"));
	
	if (SourceSlotAddress.ContainerType != ESlotType::Storage)
	{
		return;
	}
	
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotDropped Called"));
	// TODO : Shop sell
	// ShopComponentRef->TrySellItem()
}

void UShopWidget::OnCloseButtonClicked()
{
	CloseWidget();
}