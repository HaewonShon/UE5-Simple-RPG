// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/CharacterInformationWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Player/Components/EquipmentComponent.h"

#include "Blueprint/WidgetLayoutLibrary.h"

DEFINE_LOG_CATEGORY(LogCharacterInfoWidget)

void UCharacterInformationWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidgetRef)
{
	ItemDescriptionWidgetRef = DescriptionWidgetRef;
}

void UCharacterInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Equipment Slot Setup

	EquipmentSlots.Add(WeaponSlot); // 0
	EquipmentSlots.Add(HelmetSlot); // 1
	EquipmentSlots.Add(ChestSlot); // 2
	EquipmentSlots.Add(PantsSlot); // 3
	EquipmentSlots.Add(BootsSlot); // 4

	for(int32 Index = 0; Index < EquipmentSlots.Num(); ++Index)
	{
		TWeakObjectPtr<UItemSlotWidget> SlotWidget = EquipmentSlots[Index];
		SlotWidget->SetSlotAddress({ ESlotType::Equipment, Index });
		
		SlotWidget->OnDrop.BindUObject(this, &UCharacterInformationWidget::OnSlotDropped);
		SlotWidget->OnHovered.BindUObject(this, &UCharacterInformationWidget::OnSlotHovered);
		SlotWidget->OnHoverEnded.BindUObject(this, &UCharacterInformationWidget::OnSlotHoverEnded);
	}
	

	// Init component-related
	if (ASimpleRPGPlayerState* PlayerState = GetOwningPlayerState<ASimpleRPGPlayerState>())
	{
		EquipmentComponentRef = PlayerState->GetComponentByClass<UEquipmentComponent>();
		check(EquipmentComponentRef.IsValid());

		EquipmentComponentRef->OnEquipmentContentChanged.BindUObject(this, &UCharacterInformationWidget::UpdateContents);
		OnNativeVisibilityChanged.AddUObject(this, &UCharacterInformationWidget::OnWidgetToggled);
	}

	OnNativeVisibilityChanged.AddUObject(this, &UCharacterInformationWidget::OnWidgetToggled);
}

void UCharacterInformationWidget::OnWidgetToggled(ESlateVisibility ChangedVisibility)
{
	if (ChangedVisibility == ESlateVisibility::Visible)
	{
		UpdateContents();
	}
}

void UCharacterInformationWidget::OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	if (SourceSlotAddress.ContainerType != ESlotType::Storage)
	{
		return;
	}

	// Inventory -> Equipment
	EquipmentComponentRef->RequestEquip(SourceSlotAddress.SlotIndex, TargetSlotAddress.SlotIndex);
}

void UCharacterInformationWidget::OnSlotHovered(const FSlotAddress& SlotAddress)
{
	UE_LOG(LogCharacterInfoWidget, Verbose, TEXT("UCharacterInformationWidget OnSlotHovered Called"));
		
	if (EquipmentSlots[SlotAddress.SlotIndex]->IsEmpty())
	{
		return;
	}

	FItemDescription Description;
	Description = EquipmentComponentRef->GetItemDescription(static_cast<EEquipmentType>(SlotAddress.SlotIndex));

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInScreen(MousePos);
}

void UCharacterInformationWidget::OnSlotHoverEnded()
{
	UE_LOG(LogCharacterInfoWidget, Verbose, TEXT("Inventory Widget OnSlotHoverEnded Called"));
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UCharacterInformationWidget::UpdateContents()
{
	UE_LOG(LogCharacterInfoWidget, Verbose, TEXT("UCharacterInformationWidget UpdateContents Called"));
	if (GetVisibility() != ESlateVisibility::Collapsed)
	{
		UpdateEquipmentSlots();
	}
}

void UCharacterInformationWidget::UpdateEquipmentSlots()
{
	for (int32 Index = 0; Index < static_cast<int32>(EEquipmentType::Count); ++Index)
	{
		FInventorySlot& EquipmentSlot = EquipmentComponentRef->GetEquipmentSlot(static_cast<EEquipmentType>(Index));
		if (!EquipmentSlot.IsEmpty())
		{
			FSlotContent Content(EquipmentSlot.Item.DataAsset.Get());;
			Content.SlotAddress = FSlotAddress{ ESlotType::Equipment, Index };
			EquipmentSlots[Index]->UpdateSlot(Content);
		}
		else
		{
			EquipmentSlots[Index]->ClearItem();
		}
	}
}
