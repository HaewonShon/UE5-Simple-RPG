// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "ShopWidget.generated.h"

/**
 *	A Widget class for NPC shop
 */
UCLASS()
class SIMPLERPG_API UShopWidget : public USessionWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitializeShop(class UShopComponent* ShopComponent);

	void SetDescriptionWidgetRef(class UItemDescriptionWidget* DescriptionWidget);

protected:
	void UpdateShopContents();

	//void RequestBuyItem();
	//void RequestSellItem();

	UFUNCTION()
	void OnSlotDoubleClicked(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHovered(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHoverEnded();

	UFUNCTION()
	void OnCloseButtonClicked();

	/**************************
	*   Widget Properties
	***************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UItemSlotWidget> SlotWidgetClass;

	/************************
	*   Bind Widgets
	*************************/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CloseButton;

	/************************
	*   Others
	*************************/
	UPROPERTY()
	TWeakObjectPtr<class UShopComponent> ShopComponentRef;


	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;
};
