// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Shop/UI/ShopWidget.h"
#include "../ShopComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"

#include "Shared/Item/UI/ItemSlotDragWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"
#include "Shared/Item/UI/ItemGridWidget.h"


void UShopWidget::NativeConstruct()
{
	// Slot Setup
	if (VenderSlotGridPanel && SlotWidgetClass.Get())
	{
		VenderSlotGridPanel->SetSlotType(ESlotType::Shop);

		/*for (int32 h = 0; h < PageHeight; ++h)
		{
			for (int32 w = 0; w < PageWidth; ++w)
			{
				UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
				VenderSlotGridPanel->AddChildToUniformGrid(SlotWidget, h, w);
				if (SlotWidget)
				{
					SlotWidget->OnDoubleClick.BindUObject(this, &UShopWidget::OnSlotDoubleClicked);
					SlotWidget->OnHovered.BindUObject(this, &UShopWidget::OnSlotHovered);
					SlotWidget->OnHoverEnded.BindUObject(this, &UShopWidget::OnSlotHoverEnded);
					SlotWidget->SetIndex(h * PageWidth + w);
					SlotWidget->SlotType = ESlotType::Shop;
				}
				else
				{
					UE_LOG(LogShop, Warning, TEXT("Failed to bind drag function."));
				}
			}
		}*/
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
	ShopComponentRef->OnShopContentChanged.BindUObject(this, &UShopWidget::UpdateShopContents);
	UpdateShopContents();
}

void UShopWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidget)
{
	ItemDescriptionWidgetRef = DescriptionWidget;
}

void UShopWidget::UpdateShopContents()
{
	const TArray<FShopItem>& ShopItemList = ShopComponentRef->GetShopItemList();
	for(int32 Index = 0; Index < ShopItemList.Num(); ++Index)
	{
		const FShopItem& ShopItem = ShopItemList[Index];
		if (ShopItem.Item.ItemData)
		{
			UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(VenderSlotGridPanel->GetSlotAt(Index));
			SlotWidget->SetItem(ShopItem.Item);
		}
	}
}

void UShopWidget::OnSlotDoubleClicked(FSlotInfo SlotWidget)
{
	// Buy Request
	// create/push quantity confirm widget
	// save current slot's item id for request
	int32 Index = SlotWidget.SlotIndex;

	ShopComponentRef->RequestPurchaseItem(Index);
}

void UShopWidget::OnSlotHovered(FSlotInfo SlotWidget)
{
	// display item info & gold info
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotHovered Called"));
	FItemDescription Description = ShopComponentRef->GetItemDescription(SlotWidget.SlotIndex);

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

void UShopWidget::OnCloseButtonClicked()
{
	CloseWidget();
}