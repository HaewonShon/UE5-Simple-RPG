// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SlotWidget.h"
#include "ItemSlotWidget.generated.h"

/**
 *	 Widget for each item slot in inventory.
 */
UCLASS()
class SIMPLERPG_API UItemSlotWidget : public USlotWidget
{
	GENERATED_BODY()

public:
	virtual void UpdateSlot(const FSlotContent& NewSlotContent) override;
	void ClearItem();

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Background;

	FLinearColor DefaultBackgroundColor;
};
