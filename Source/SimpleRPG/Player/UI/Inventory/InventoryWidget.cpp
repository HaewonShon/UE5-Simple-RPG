// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "BackdropWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "Shared/Item/UI/ItemSlotDragWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Shared/Item/UI/ItemGridWidget.h"

#include "Components/Border.h"
#include "Components/InvalidationBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // UDragDropOperation
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"

#include "Player/SimpleRPGPlayerState.h"
#include "Player/Components/CurrencyComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GridWidget->SetSlotType(ESlotType::Storage);

	GridWidget->OnDragBegin.AddUObject(this, &UInventoryWidget::OnSlotDragBegin);
	GridWidget->OnDoubleClick.AddUObject(this, &UInventoryWidget::OnItemUsed);
	GridWidget->OnHovered.AddUObject(this, &UInventoryWidget::OnSlotHovered);
	GridWidget->OnDrop.AddUObject(this, &UInventoryWidget::OnSlotsSwapped);
	GridWidget->OnHoverEnded.AddUObject(this, &UInventoryWidget::OnSlotHoverEnded);

	// Equipment Slot Setup
	/*{
		EquipmentSlotMap.Add({ ESlotType::Helmet, HelmetSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Chest, ChestSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Pants, PantsSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Boots, BootsSlot.Get() });
		EquipmentSlotMap.Add({ ESlotType::Weapon, WeaponSlot.Get() });

		for(ESlotType SlotType : TEnumRange<ESlotType>())
		{
			if (SlotType == ESlotType::Storage || SlotType == ESlotType::Shop) continue;

			UItemSlotWidget* SlotWidget = EquipmentSlotMap[SlotType];
			SlotWidget->SlotType = SlotType;
			SlotWidget->OnDragBegin.BindUObject(this, &UInventoryWidget::OnSlotDragBegin);
			SlotWidget->OnDrop.BindUObject(this, &UInventoryWidget::OnSlotsSwapped);
			SlotWidget->OnDoubleClick.BindUObject(this, &UInventoryWidget::OnItemUsed);
			SlotWidget->OnHovered.BindUObject(this, &UInventoryWidget::OnSlotHovered);
			SlotWidget->OnHoverEnded.BindUObject(this, &UInventoryWidget::OnSlotHoverEnded);
		}
	}*/

	// Init component-related
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		InventoryComponentRef = PlayerState->GetInventoryComponent();
		check(InventoryComponentRef.IsValid());

		InventoryComponentRef->OnInventoryContentChanged.BindUObject(this, &UInventoryWidget::UpdateContents);
		InventoryComponentRef->OnEquipmentContentChanged.BindUObject(this, &UInventoryWidget::UpdateEquipmentSlotWidgets);
		UE_LOG(LogInventory, Log, TEXT("Inventory Component bound to ui"));
		
		OnNativeVisibilityChanged.AddUObject(this, &UInventoryWidget::OnInventoryToggled);

		if (UCurrencyComponent* CurrencyComponent = PlayerState->GetComponentByClass<UCurrencyComponent>())
		{
			CurrencyComponent->OnGoldAmountChanged.AddUObject(this, &UInventoryWidget::UpdateGoldAmount);
		}
	}

	// Inventory Widget Initialization
	SlotVisualWidget = CreateWidget<UItemSlotDragWidget>(GetOwningPlayer(), SlotVisualWidgetClass);
	if (SlotVisualWidget)
	{
		SlotVisualWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create SlotVisualWidget"));
	}

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

void UInventoryWidget::OnSlotDragBegin(const FSlotAddress& SlotAddress)
{
	USlotWidget* SlotWidget = GridWidget->GetSlotAt(SlotAddress.SlotIndex);
	if (!SlotWidget)
	{
		UE_LOG(LogInventory, Warning, TEXT("Failed to cast InventorySlotWidget"));
		return;
	}

	UDragDropOperation*& DragOperation = SlotWidget->DragDropOperationRef;
	if (DragOperation)
	{
		DragOperation->DefaultDragVisual = SlotVisualWidget;
		SlotVisualWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SlotVisualWidget->OnDragBegin(SlotWidget->GetIconTexture());
		SlotVisualWidget->SetDesiredSize(FVector2D{64.f, 64.f});

	}
	else
	{
		UE_LOG(LogInventory, Warning, TEXT("DragOperation not valid"));
	}
}

void UInventoryWidget::OnSlotsSwapped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	if (InventoryComponentRef->GetCurrentMode() == EInventoryMode::Shop)
	{
		if (SourceSlotAddress.ContainerType == ESlotType::Shop)
		{
			// Buy Request
		}
	}
	else
	{
		// equipment -> inventory
		if (SourceSlotAddress.ContainerType != ESlotType::Storage)
		{
			return;
		}
		else
		{
			InventoryComponentRef->SwapItems(SourceSlotAddress.SlotIndex, TargetSlotAddress.SlotIndex);
		}
		
		UpdateCurrentPageContents();
	}	
}

void UInventoryWidget::OnItemDiscarded(const FSlotAddress& SlotAddress)
{
	UE_LOG(LogInventory, Log, TEXT("Widget OnItemDiscard %i"), SlotAddress.SlotIndex);
	InventoryComponentRef->RemoveItem(SlotAddress.SlotIndex, true);
}

void UInventoryWidget::OnItemUsed(const FSlotAddress& SlotAddress)
{
	UE_LOG(LogInventory, Verbose, TEXT("Widget OnItemUsed %i"), SlotAddress.SlotIndex);

	if (SlotAddress.ContainerType != ESlotType::Storage)
	{
		//InventoryComponentRef->TryRemoveEquipment(ConvertSlotTypeToEquipmentType(SlotWidget.SlotType));
	}
	// TODO : Re-write equipment equip logic
	//else if (SelectedPage == EInventoryCategory::Equipment)
	//{
	//	InventoryComponentRef->TryEquipItem(SlotWidget.SlotIndex);
	//}
	else
	{
		InventoryComponentRef->UseItem(SlotAddress.SlotIndex);
	}
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
	}
}

void UInventoryWidget::UpdateEquipmentSlotWidgets()
{
	/*EEquipmentType Types[5] = 
		{ EEquipmentType::Helmet, EEquipmentType::Chest, EEquipmentType::Pants, EEquipmentType::Boots, EEquipmentType::Weapon };
	ESlotType SlotTypes[5] =
		{ ESlotType::Helmet, ESlotType::Chest, ESlotType::Pants, ESlotType::Boots, ESlotType::Weapon };

	for (int32 i = 0; i < 5; ++i)
	{
		const FInventorySlot& ItemSlot = InventoryComponentRef->GetEquipmentSlot(Types[i]);
		UItemSlotWidget* SlotWidget = EquipmentSlotMap[SlotTypes[i]];

		if (!ItemSlot.IsEmpty())
		{
			SlotWidget->SetItem(ItemSlot.Item);
		}
		else
		{
			SlotWidget->ClearItem();
		}
	}*/
}

void UInventoryWidget::UpdateCurrentPageContents()
{
	TArray<FInventorySlot>& InventorySlots = InventoryComponentRef->GetPage().Slots;
	TArray<UWidget*> SlotWidgets = GridWidget->GetAllSlots();
	for (int32 Index = 0; Index < InventorySlots.Num(); ++Index)
	{
		UItemSlotWidget* SlotWidget = Cast<UItemSlotWidget>(SlotWidgets[Index]);
		FInventorySlot& ItemSlot = InventorySlots[Index];

		if (!ItemSlot.IsEmpty())
		{
			FSlotContent Content(ItemSlot.Item.DataAsset.Get());;
			Content.SlotAddress = FSlotAddress{ ESlotType::Storage, Index };
			SlotWidget->UpdateSlot(Content);
		}
		else
		{
			SlotWidget->ClearItem();
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