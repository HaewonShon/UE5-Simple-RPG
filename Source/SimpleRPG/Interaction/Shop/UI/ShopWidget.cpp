// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Shop/UI/ShopWidget.h"
#include "../ShopComponent.h"
#include "Components/UniformGridPanel.h"

#include "Shared/Item/UI/ItemSlotDragWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"

void UShopWidget::NativeConstruct()
{
	// Slot Setup
	if (SlotGridPanel && SlotWidgetClass.Get())
	{
		for (int32 h = 0; h < PageHeight; ++h)
		{
			for (int32 w = 0; w < PageWidth; ++w)
			{
				UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), SlotWidgetClass);
				SlotGridPanel->AddChildToUniformGrid(SlotWidget, h, w);
				if (SlotWidget)
				{
					SlotWidget->OnDragBegin.BindUObject(this, &UShopWidget::OnSlotDragBegin);
					SlotWidget->OnDrop.BindUObject(this, &UShopWidget::OnSlotsSwapped);
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
		}
	}
	else
	{
		UE_LOG(LogShop, Warning, TEXT("Failed to create Item Slots, %i, %i"), SlotGridPanel == nullptr, SlotWidgetClass == nullptr);
	}

	SlotVisualWidget = CreateWidget<UItemSlotDragWidget>(GetOwningPlayer(), SlotVisualWidgetClass);
	if (SlotVisualWidget)
	{
		SlotVisualWidget->SetDesiredSize(FVector2D{ SlotGridPanel->GetMinDesiredSlotWidth(), SlotGridPanel->GetMinDesiredSlotHeight() });
		SlotVisualWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShopWidget::InitializeShop(UShopComponent* ShopComponent)
{
	if (!ShopComponent)
	{
		UE_LOG(LogShop, Warning, TEXT("ShopWidget Initialize: Given shop component is not valid"));
		return;
	}

	ShopComponentRef = ShopComponent;
	UpdateShopContents();
}

void UShopWidget::SetDescriptionWidgetRef(UUserWidget* DescriptionWidgetRef)
{
	if (UItemDescriptionWidget* DescriptionWidget = Cast<UItemDescriptionWidget>(DescriptionWidgetRef))
	{
		ItemDescriptionWidgetRef = DescriptionWidget;
	}
	else
	{
		UE_LOG(LogShop, Warning, TEXT("ShopWidget Description pointer is not valid"));
	}
}

void UShopWidget::UpdateShopContents()
{
	const TMap<FPrimaryAssetId, FShopItem>& ShopItems = ShopComponentRef->GetShopItems();
	int32 Index = 0;
	for(const auto& Pair : ShopItems)
	{
		const FShopItem& ShopItem = Pair.Value;
		if (ShopItem.Item.ItemData)
		{
			UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(SlotGridPanel->GetChildAt(Index));
			SlotWidget->SetItem(ShopItem.Item);
		}
	}
}

void UShopWidget::OnSlotDragBegin(FSlotInfo SlotInfo)
{
	UItemSlotWidget* SlotWidget;
	SlotWidget = Cast<UItemSlotWidget>(SlotGridPanel->GetChildAt(SlotInfo.SlotIndex));

	if (!SlotWidget)
	{
		UE_LOG(LogShop, Warning, TEXT("Failed to cast UItemSlotWidget"));
		return;
	}

	UDragDropOperation*& DragOperation = SlotWidget->DragDropOperationRef;
	if (DragOperation)
	{
		DragOperation->DefaultDragVisual = SlotVisualWidget;
		SlotVisualWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SlotVisualWidget->OnDragBegin(SlotWidget->GetIconTexture());
	}
	else
	{
		UE_LOG(LogShop, Warning, TEXT("DragOperation not valid"));
	}
}

void UShopWidget::OnSlotsSwapped(FSlotInfo Slot1, FSlotInfo Slot2)
{
	if (Slot1.SlotType == ESlotType::Storage && Slot2.SlotType == ESlotType::Shop)
	{
		// Sell request
	}
}

void UShopWidget::OnSlotDoubleClicked(FSlotInfo SlotWidget)
{
	// Buy Request
	
}

void UShopWidget::OnSlotHovered(FSlotInfo SlotWidget)
{
	// display item info & gold info
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotHovered Called"));
	FItemDescription Description = ShopComponentRef->GetItemDescription(SlotWidget.SlotIndex);

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInViewport(MousePos, false);
}

void UShopWidget::OnSlotHoverEnded()
{
	UE_LOG(LogShop, Verbose, TEXT("UShopWidget OnSlotHoverEnded Called"));
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}
