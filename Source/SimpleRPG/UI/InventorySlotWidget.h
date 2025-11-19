// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 *	 Widget for each item slot in inventory.
 */
UCLASS()
class SIMPLERPG_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Setter for Item Image and stack count text */
	void SetItem(const struct FItemInstance* Item);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> StackText;
};
