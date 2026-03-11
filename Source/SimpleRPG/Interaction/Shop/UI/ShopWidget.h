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

	/************************
	*	Common slot methods
	* **********************/
	UFUNCTION()
	void OnSlotHovered(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHoverEnded();
	
	UFUNCTION()
	void OnSlotDropped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress);

	UFUNCTION()
	void OnCloseButtonClicked();

	/**************************
	*   Widget Properties
	***************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int32 PageHeight;
	
	/************************
	*   Vender-related Widgets
	*************************/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemGridWidget> GridWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<class UShopSlotWidget> SlotWidgetClass;


	/************************
	*   Player-related Widgets
	*************************/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventoryWidget> InventoryWidget;

	

	/************************
	*   Others
	*************************/
	UPROPERTY()
	TWeakObjectPtr<class UShopComponent> ShopComponentRef;

	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> InventoryComponentRef;

	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;
};
