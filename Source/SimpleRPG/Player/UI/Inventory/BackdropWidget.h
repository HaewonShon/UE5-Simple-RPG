// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "BackdropWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnItemDiscard, FSlotInfo);

/**
 *	Invisible widget to receive drop input
 */
UCLASS(BlueprintType)
class SIMPLERPG_API UBackdropWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnItemDiscard OnItemDiscard;

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
