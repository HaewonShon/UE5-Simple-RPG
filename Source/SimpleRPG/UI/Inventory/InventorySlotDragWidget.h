// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Item//ItemData.h"
#include "InventorySlotDragWidget.generated.h"

/**
 *  Visualizer for inventory slot dragging
 */
UCLASS()
class SIMPLERPG_API UInventorySlotDragWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    void OnDragBegin(UTexture2D* Texture);
    void OnDragEnd();
    void SetDesiredSize(FVector2D Size);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class USizeBox> SizeBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> ItemIcon;
};
