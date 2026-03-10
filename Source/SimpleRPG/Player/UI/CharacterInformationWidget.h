// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "CharacterInformationWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacterInfoWidget, Log, All);

/**
 *	Widget to display charater's status, including equipment & current stats
 */
UCLASS()
class SIMPLERPG_API UCharacterInformationWidget : public USessionWidget
{
	GENERATED_BODY()
	
public:
	void SetDescriptionWidgetRef(class UItemDescriptionWidget* DescriptionWidgetRef);
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnWidgetToggled(ESlateVisibility ChangedVisibility);

	UFUNCTION()
	void OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress);
	
	UFUNCTION()
	void OnSlotHovered(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHoverEnded();

	UFUNCTION()
	void UpdateContents();
	void UpdateEquipmentSlots();

	///**** Equipment Slots ****/
	UPROPERTY()
	TArray<TWeakObjectPtr<class UItemSlotWidget>> EquipmentSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> HelmetSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> ChestSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> PantsSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> BootsSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> WeaponSlot;

	/************************
	*   Other members for inventory widget
	*************************/
	UPROPERTY()
	TWeakObjectPtr<class UEquipmentComponent> EquipmentComponentRef;

	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;

	bool bIsContentChanged;
};
