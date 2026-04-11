// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "Shared/Item/ItemManagementSubsystem.h"
#include "EnhancementWidget.generated.h"

UENUM()
enum class EEnhanceState : uint8
{
	Idle,
	Pending,
	Processing,
	Success,
	Fail,
};

/**
 *  Widget for enhancement system
 */
UCLASS()
class SIMPLERPG_API UEnhancementWidget : public USessionWidget
{
	GENERATED_BODY()
	
public:
	void SetDescriptionWidgetRef(class UItemDescriptionWidget* DescriptionWidget);

	UFUNCTION()
	void ClearItemSlot(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void ResetState();

	UFUNCTION(BlueprintCallable)
	void SetStateProcessing();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateContent();

	UFUNCTION()
	void OnEnhanceButtonClicked();

	UFUNCTION()
	void ProcessEnhanceResult(EEnhanceResult Result);

	void SetEnhanceDisplayStatus(bool bIsEnable);
	void SetCurrentItemDisplay(const FItemDescription& Description);
	void SetPreviewItemDisplay(const FItemDescription& Description);
	void ClearCurrentItemDisplay();
	void ClearPreviewItemDisplay();
	void ClearRequirementsDisplay();

	UFUNCTION()
	void OnSlotHovered(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHoverEnded();

	UFUNCTION()
	void OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress);

	UFUNCTION()
	void OnCloseButtonClicked();

	/**************************
	*   Widget Properties
	***************************/
	EEnhanceState State;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemSlotWidget> ItemSlot;

	/*** enhancement info display ***/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ChanceText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CurrentItemTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CurrentItemStat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PreviewItemTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PreviewItemStat;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ArrowImageButton;

	/*** Enhance requirement display ***/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> RequirementSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEnhanceRequirementSlotWidget> GoldRequirementWidget;

	/*** Button widgets ***/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EnhanceButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton;

	/************************
	*	Visuals
	*************************/
	void PlayEffect(EEnhanceState NewState);
	bool bHasResponsee;
	EEnhanceResult Response;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PendingAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SuccessAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FailAnim;

	/************************
	*   Others
	*************************/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEnhanceRequirementSlotWidget> RequirementWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UActionableAsset> GoldDisplayAsset;

	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;

	UPROPERTY()
	TWeakObjectPtr<class UItemManagementSubsystem> ItemManagementSubsystemRef;
};
