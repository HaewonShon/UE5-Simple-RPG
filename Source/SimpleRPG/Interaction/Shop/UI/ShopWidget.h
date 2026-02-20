// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "ShopWidget.generated.h"

/**
 *	A Widget class for NPC shop
 */
UCLASS()
class SIMPLERPG_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void Initialize(class UShopComponent* ShopComponent);

	void SetDescriptionWidgetRef(UUserWidget* DescriptionWidgetRef);

protected:
	void UpdateShopContents();

	//void RequestBuyItem();
	//void RequestSellItem();

	UFUNCTION()
	void OnSlotDragBegin(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotsSwapped(FSlotInfo Slot1, FSlotInfo Slot2);

	UFUNCTION()
	void OnSlotDoubleClicked(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHovered(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHoverEnded();

	/**************************
	*   Widget Properties
	***************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UItemSlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UItemSlotDragWidget> SlotVisualWidgetClass;

	/************************
	*   Bind Widgets
	*************************/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;

	/************************
	*   Others
	*************************/
	UPROPERTY()
	TWeakObjectPtr<class UShopComponent> ShopComponentRef;

	UPROPERTY()
	TObjectPtr<class UItemSlotDragWidget> SlotVisualWidget;

	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;
};
