// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "BackdropWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Shared/Item/UI/ItemGridWidget.h"

#include "Components/Border.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

//#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"

#include "Player/SimpleRPGPlayerState.h"
#include "Player/Components/CurrencyComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GridWidget->SetSlotType(ESlotType::Storage);

	//GridWidget->OnDragBegin.AddUObject(this, &UInventoryWidget::OnSlotDragBegin);
	//GridWidget->OnDoubleClick.AddUObject(this, &UInventoryWidget::OnItemUsed);
	GridWidget->OnHovered.AddUObject(this, &UInventoryWidget::OnSlotHovered);
	GridWidget->OnDrop.AddUObject(this, &UInventoryWidget::OnSlotsSwapped);
	GridWidget->OnHoverEnded.AddUObject(this, &UInventoryWidget::OnSlotHoverEnded);

	// Init component-related
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		InventoryComponentRef = PlayerState->GetComponentByClass<UInventoryComponent>();
		check(InventoryComponentRef.IsValid());

		InventoryComponentRef->OnInventoryContentChanged.AddUObject(this, &UInventoryWidget::UpdateContents);		
		OnNativeVisibilityChanged.AddUObject(this, &UInventoryWidget::OnInventoryToggled);

		if (UCurrencyComponent* CurrencyComponent = PlayerState->GetComponentByClass<UCurrencyComponent>())
		{
			CurrencyComponent->OnGoldAmountChanged.AddUObject(this, &UInventoryWidget::UpdateGoldAmount);
			UpdateGoldAmount(CurrencyComponent->GetCurrencyAmount(ECurrencyType::Gold));
		}
		else
		{
			UE_LOG(LogInventory, Warning, TEXT("Failed to get currency component from playerstate"));
		}
	}
	UpdateCurrentPageContents();
	//CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseButtonClicked);
}

void UInventoryWidget::BindItemDiscardDelegate(UBackdropWidget* Widget)
{
	if (!Widget)
	{
		UE_LOG(LogInventory, Warning, TEXT("Backdrop widget is not valid"));
		return;
	}

	Widget->OnItemDiscard.BindUObject(this, &UInventoryWidget::OnItemDiscarded);
	UE_LOG(LogInventory, Verbose, TEXT("OnItemDiscard Registered"));
}

void UInventoryWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidgetRef)
{
	ItemDescriptionWidgetRef = DescriptionWidgetRef;
}


void UInventoryWidget::OnCurrentPageSort()
{
	
}

void UInventoryWidget::OnInventoryToggled(ESlateVisibility ChangedVisibility)
{
	if (ChangedVisibility == ESlateVisibility::Visible)
	{
		UpdateCurrentPageContents();
	}
}

void UInventoryWidget::OnSlotsSwapped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	if (InventoryComponentRef->GetCurrentMode() == EInventoryMode::Shop)
	{
		if (SourceSlotAddress.ContainerType == ESlotType::Shop)
		{
			InventoryComponentRef->RequestPurchaseItem(SourceSlotAddress.SlotIndex);
		}
	}
	else
	{
		if (SourceSlotAddress.ContainerType == ESlotType::Storage)
		{
			InventoryComponentRef->SwapItems(SourceSlotAddress.SlotIndex, TargetSlotAddress.SlotIndex);
		}
		else if (SourceSlotAddress.ContainerType == ESlotType::Equipment)
		{
			InventoryComponentRef->RequestRemoveEquipment(SourceSlotAddress.SlotIndex, TargetSlotAddress.SlotIndex);
		}
		UpdateCurrentPageContents();
	}	
}

void UInventoryWidget::OnItemDiscarded(const FSlotAddress& SlotAddress)
{
	UE_LOG(LogInventory, Log, TEXT("Widget OnItemDiscard %i"), SlotAddress.SlotIndex);
	InventoryComponentRef->RemoveItem(SlotAddress.SlotIndex, true);
}

void UInventoryWidget::OnSlotHovered(const FSlotAddress& SlotAddress)
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnSlotHovered Called"));

	if (InventoryComponentRef->GetPage().IsSlotEmpty(SlotAddress.SlotIndex))
	{
		return;
	}

	FItemDescription Description;
	Description = InventoryComponentRef->GetItemDescription(SlotAddress.SlotIndex);

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInScreen(MousePos);
}

void UInventoryWidget::OnSlotHoverEnded()
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnSlotHoverEnded Called"));
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::UpdateContents()
{
	UE_LOG(LogInventory, Verbose, TEXT("Inventory Widget OnChanged Called"));
	if (GetVisibility() != ESlateVisibility::Collapsed)
	{
		UpdateCurrentPageContents();
		OnSlotHoverEnded(); // hide description widget if exist
	}
}

void UInventoryWidget::UpdateCurrentPageContents()
{
	if (!InventoryComponentRef.IsValid())
	{
		return;
	}

	TArray<FInventorySlot>& InventorySlots = InventoryComponentRef->GetPage().Slots;
	TArray<UWidget*> SlotWidgets = GridWidget->GetAllSlots();
	for (int32 Index = 0; Index < InventorySlots.Num(); ++Index)
	{
		UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(SlotWidgets[Index]);
		FInventorySlot& ItemSlot = InventorySlots[Index];

		if (!ItemSlot.IsEmpty())
		{
			FSlotContent Content(ItemSlot.Item.DataAsset.Get());;
			Content.Quantity = ItemSlot.Item.StackCount;
			Content.SlotAddress = FSlotAddress{ ESlotType::Storage, Index };
			SlotWidget->UpdateSlot(Content);
		}
		else
		{
			SlotWidget->ClearSlot();
		}
	}
}

void UInventoryWidget::UpdateGoldAmount(int32 Amount)
{
	FNumberFormattingOptions Opts;
	Opts.UseGrouping = true;
	Opts.MinimumIntegralDigits = 1;

	FText FormattedText = FText::AsNumber(Amount, &Opts);
	GoldDisplayText->SetText(FormattedText);
}

void UInventoryWidget::OnCloseButtonClicked()
{
	if (InventoryComponentRef->GetCurrentMode() == EInventoryMode::Shop)
	{
		return;
	}

	CloseWidget();
}