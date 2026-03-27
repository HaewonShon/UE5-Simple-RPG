// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Enhancement/UI/EnhancementWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Player/SimpleRPGPlayerState.h"
#include "Player/Components/EquipmentComponent.h"
#include "Shared/Item/UI/ItemSlotDragWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Player/UI/Inventory/InventoryWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Misc/OutputDeviceDebug.h"

void UEnhancementWidget::SetDescriptionWidgetRef(UItemDescriptionWidget* DescriptionWidget)
{
	ItemDescriptionWidgetRef = DescriptionWidget;
}

void UEnhancementWidget::ClearItemSlot(const FSlotAddress& SlotAddress)
{
	ItemManagementSubsystemRef->SetEnhanceTargetItem(nullptr);

	SetEnhanceButtonStatus(false);
	ClearCurrentItemDisplay();
	ClearPreviewItemDisplay();
}

void UEnhancementWidget::ResetState()
{
	State = EEnhanceState::Idle;
	bHasResponsee = false;

	UpdateContent();
}

void UEnhancementWidget::SetStateProcessing()
{
	UE_LOG(LogTemp, Warning, TEXT("UEnhancementWidget::SetStateProcessing"));
	State = EEnhanceState::Processing;
	if (bHasResponsee)
	{
		ProcessEnhanceResult(Response);
	}
}	

void UEnhancementWidget::OnSlotHovered(const FSlotAddress& SlotAddress)
{
	FItemInstance* TargetItem = ItemManagementSubsystemRef->GetEnhanceTargetItem();
	if (!TargetItem || !TargetItem->IsValid())
	{
		return;
	}

	FItemDescription Description = TargetItem->DataAsset->BuildDescriptionData();

	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::HitTestInvisible);
	ItemDescriptionWidgetRef->SetDescription(Description);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	ItemDescriptionWidgetRef->SetPositionInScreen(MousePos);
}

void UEnhancementWidget::OnSlotHoverEnded()
{
	ItemDescriptionWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
}

void UEnhancementWidget::OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress)
{
	UE_LOG(LogTemp, Warning, TEXT("UEnhancementWidget::OnSlotDropped"));
	if (SourceSlotAddress.ContainerType == ESlotType::Storage)
	{
		if (UInventoryComponent* Inventory = GetOwningPlayerState()->GetComponentByClass<UInventoryComponent>())
		{
			Inventory->RequestRegisterEnhanceTarget(SourceSlotAddress.SlotIndex);
		}
	}
	else if (SourceSlotAddress.ContainerType == ESlotType::Equipment)
	{
		if (UEquipmentComponent* Equipment = GetOwningPlayerState()->GetComponentByClass<UEquipmentComponent>())
		{
			Equipment->RequestRegisterEnhanceTarget(SourceSlotAddress.SlotIndex);
		}
	}
}

void UEnhancementWidget::OnCloseButtonClicked()
{
	CloseWidget();
}

void UEnhancementWidget::NativeConstruct()
{
	ItemManagementSubsystemRef = GetGameInstance()->GetSubsystem<UItemManagementSubsystem>();
	check(ItemManagementSubsystemRef.IsValid());

	ItemManagementSubsystemRef->OnEnhanceTargetChanged.AddUObject(this, &UEnhancementWidget::UpdateContent);
	ItemSlot->SetSlotAddress({ESlotType::Enhancement, 0});

	// Item slot event binding
	ItemSlot->OnDoubleClick.AddUObject(this, &UEnhancementWidget::ClearItemSlot);
	ItemSlot->OnDrop.AddUObject(this, &UEnhancementWidget::OnSlotDropped);
	ItemSlot->OnHovered.AddUObject(this, &UEnhancementWidget::OnSlotHovered);
	ItemSlot->OnHoverEnded.AddUObject(this, &UEnhancementWidget::OnSlotHoverEnded);
	CloseButton->OnClicked.AddDynamic(this, &UEnhancementWidget::OnCloseButtonClicked);
	
	// enhance button binding
	EnhanceButton->OnClicked.AddDynamic(this, &UEnhancementWidget::OnEnhanceButtonClicked);

	// temp clear
	ClearItemSlot({});
	ResetState();

	ItemManagementSubsystemRef->OnEnhanceCompleted.AddUObject(this, &UEnhancementWidget::ProcessEnhanceResult);

	FWidgetAnimationDynamicEvent Event;
	Event.BindUFunction(this, FName("ResetState"));
	BindToAnimationFinished(SuccessAnim, Event);
	BindToAnimationFinished(FailAnim, Event);

}
	
void UEnhancementWidget::UpdateContent()
{
	FItemInstance* Target = ItemManagementSubsystemRef->GetEnhanceTargetItem();
	if (!Target || !Target->IsValid())
	{
		ItemSlot->ClearSlot();

		ArrowImageButton->SetIsEnabled(false);
		EnhanceButton->SetIsEnabled(false);
	}
	else
	{
		// set slot image
		FSlotContent Content(Target->DataAsset.Get());
		Content.SlotAddress = FSlotAddress{ ESlotType::Enhancement, 0 };
		ItemSlot->UpdateSlot(Content);

		SetCurrentItemDisplay(Target->BuildDescriptionData());

		FEnhanceDisplayInfo DisplayInfo;
		if (ItemManagementSubsystemRef->GetEnhanceData(GetOwningPlayerState(), DisplayInfo))
		{
			SetEnhanceButtonStatus(true);

			ChanceText->SetText(FText::AsPercent(DisplayInfo.SuccessRate));
			SetPreviewItemDisplay(DisplayInfo.PreviewDescription);

			//FText Gold = FText::Format(FText::FromString("{0} // {1}"), DisplayInfo.OwningGold, DisplayInfo.GoldCost);

		}
		else // enhancement not available
		{
			SetEnhanceButtonStatus(false);
		}
	}
}

void UEnhancementWidget::OnEnhanceButtonClicked()
{
	State = EEnhanceState::Pending;
	PlayEffect(State);

	UE_LOG(LogTemp, Warning, TEXT("OnEnhanceButtonClicked"));
	ItemManagementSubsystemRef->RequestEnhanceCurrentItem(GetOwningPlayerState());
}

void UEnhancementWidget::ProcessEnhanceResult(EEnhanceResult Result)
{
	if (State == EEnhanceState::Pending)
	{
		bHasResponsee = true;
		Response = Result;
		return;
	}

	if (Result == EEnhanceResult::Success)
	{
		// TODO : Success Animation
		State = EEnhanceState::Success;
		PlayEffect(State);

	}
	else if (Result == EEnhanceResult::Fail)
	{
		// TODO : Fail Aniimation
		State = EEnhanceState::Fail;
		PlayEffect(State);
	}
	else
	{
		ResetState();
	}
}

void UEnhancementWidget::SetEnhanceButtonStatus(bool bIsEnable)
{
	if (bIsEnable)
	{
		ArrowImageButton->SetIsEnabled(true);
		EnhanceButton->SetIsEnabled(true);
		ChanceText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ArrowImageButton->SetIsEnabled(false);
		EnhanceButton->SetIsEnabled(false);
		ChanceText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UEnhancementWidget::SetCurrentItemDisplay(const FItemDescription& Description)
{
	CurrentItemTitle->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CurrentItemStat->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	CurrentItemTitle->SetText(Description.Name);

	FEquipmentDetail EquipmentDetail = Description.Payload.Get<FEquipmentDetail>();

	FTextBuilder Builder;
	for (const TPair<EStat, FText>& Stat : EquipmentDetail.Stats)
	{
		Builder.AppendLine(
			StaticEnum<EStat>()->GetDisplayNameTextByValue((int64)Stat.Key).ToString()
			+ FString(": ")
			+ Stat.Value.ToString()
		);
	}
	CurrentItemStat->SetText(Builder.ToText());
}

void UEnhancementWidget::SetPreviewItemDisplay(const FItemDescription& Description)
{
	PreviewItemTitle->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PreviewItemStat->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	PreviewItemTitle->SetText(Description.Name);

	FEquipmentDetail EquipmentDetail = Description.Payload.Get<FEquipmentDetail>();

	FTextBuilder Builder;
	for (const TPair<EStat, FText>& Stat : EquipmentDetail.Stats)
	{
		Builder.AppendLine(
			StaticEnum<EStat>()->GetDisplayNameTextByValue((int64)Stat.Key).ToString()
			+ FString(": ")
			+ Stat.Value.ToString()
		);
	}
	PreviewItemStat->SetText(Builder.ToText());
}

void UEnhancementWidget::ClearCurrentItemDisplay()
{
	CurrentItemTitle->SetVisibility(ESlateVisibility::Collapsed);
	CurrentItemStat->SetVisibility(ESlateVisibility::Collapsed);
}

void UEnhancementWidget::ClearPreviewItemDisplay()
{
	PreviewItemTitle->SetVisibility(ESlateVisibility::Collapsed);
	PreviewItemStat->SetVisibility(ESlateVisibility::Collapsed);
}

void UEnhancementWidget::PlayEffect(EEnhanceState NewState)
{
	StopAllAnimations();

	switch (NewState)
	{
	case EEnhanceState::Pending:
		PlayAnimation(PendingAnim, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f);
		break;

	case EEnhanceState::Success:
		PlayAnimation(SuccessAnim);
		break;

	case EEnhanceState::Fail:
		PlayAnimation(FailAnim);
		break;
	}
}