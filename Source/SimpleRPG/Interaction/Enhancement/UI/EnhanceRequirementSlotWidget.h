// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SlotWidget.h"
#include "EnhanceRequirementSlotWidget.generated.h"

/**
 *	Widget for displaying required materials in enhancement
 */
UCLASS()
class SIMPLERPG_API UEnhanceRequirementSlotWidget : public USlotWidget
{
	GENERATED_BODY()
	
public:
	virtual void UpdateSlot(const FSlotContent& NewSlotContent) override;
	void SetAmount(int32 OwningAmount, int32 RequiredAmount, bool bIsSufficient);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor InsufficientDisplayColor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> OwningAmountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RequiredAmountText;
};
