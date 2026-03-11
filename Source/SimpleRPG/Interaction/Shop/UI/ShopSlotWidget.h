// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "Shared/UI/Common/SlotWidget.h"
#include "ShopSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLERPG_API UShopSlotWidget : public UItemSlotWidget
{
	GENERATED_BODY()
	
public:
	virtual void UpdateSlot(const FSlotContent& NewSlotContent) override;
	void SetPrice(int32 Price);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> SoldOutDisplayImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> PriceText;
};
