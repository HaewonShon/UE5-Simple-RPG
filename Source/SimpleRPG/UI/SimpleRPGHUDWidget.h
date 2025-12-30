// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimpleRPGHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLERPG_API USimpleRPGHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void ToggleInventory();

	TWeakObjectPtr<UUserWidget> GetInventoryWidget() const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> MainCanvas;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBackdropWidget> BackdropWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	TObjectPtr<class UInventoryWidget> InventoryWidget;
};
