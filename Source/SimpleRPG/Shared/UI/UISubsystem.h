// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "RootWidget.h"
#include "UISubsystem.generated.h"

/**
 *  UI Management susbystem for local player
 */
UCLASS()
class SIMPLERPG_API UUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	void InitializeRootWidget(APlayerController* PC, TSubclassOf<class URootWidget> RootWidgetClass);
	UUserWidget* AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<class UUserWidget> WidgetClass, bool bFillScreen = false, bool bIsAliveAlways = false);
	
	void ToggleInventory();
	class UInventoryWidget* GetInventoryWidget() { return RootWidget->GetInventoryWidget(); }
	class UItemDescriptionWidget* GetItemDescriptionWidget() { return RootWidget->GetItemDescriptionWidget(); }

	class UItemRarityColorData* GetItemRarityColorData() const { return ItemRarityColorDataAsset; }

	class UQuantityConfirmationWidget* RequestCreateQuantityWidget();
	void RequestDisplayMessageBox(const FText& Message);
private:
	void LoadDataAssets();
	void OnWidgetDataLoaded();
	void OnItemRarityColorDataLoaded();

	UPROPERTY()
	TObjectPtr<class URootWidget> RootWidget;

	UPROPERTY()
	TObjectPtr<class UCommonWidgetData> CommonWidgetDataAsset;

	UPROPERTY()
	TObjectPtr<class UItemRarityColorData> ItemRarityColorDataAsset;
};
