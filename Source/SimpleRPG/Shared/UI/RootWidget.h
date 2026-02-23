// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RootWidget.generated.h"

UENUM()
enum class EWidgetLayer
{
	HUD,
	Dialogue = 1000,
	Menu = 1500,
	System = 2000,
};

/**
 *	A root widget for entire widgets in screen
 */
UCLASS()
class SIMPLERPG_API URootWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUserWidget* AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<UUserWidget> WidgetClass, 
		bool bFillScreen = false, bool bIsAliveAlways = false);

	void ToggleInventory();
	void NotifyWidgetRemoved(EWidgetLayer Layer);

	class UItemDescriptionWidget* GetItemDescriptionWidgetRef() { return ItemDescriptionWidget; }

private:
	virtual void NativeConstruct() override;
	void UpdateBlockingImageStatus(EWidgetLayer Layer);
	class UOverlay* GetLayer(EWidgetLayer Layer);

	/************************************************
	***    Root widget members
	************************************************/
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UOverlay> HUDLayer;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UOverlay> DialogueLayer;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UOverlay> MenuLayer;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UOverlay> SystemLayer;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> DialogueLayerBlockingImage;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> MenuLayerBlockingImage;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UImage> SystemLayerBlockingImage;

	/************************************************
	***    Widgets
	************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class USimpleRPGHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UItemDescriptionWidget> ItemDescriptionWidgetClass;

	UPROPERTY()
	TObjectPtr<class UInventoryWidget> InventoryWidget;

	UPROPERTY()
	TObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidget;

	UPROPERTY()
	TMap<EWidgetLayer, int32> LiveWidgetCountForLayer;
};
