// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 *    Inventory Component for character, support per-page item add/use/sort, etc.
 */
UCLASS()
class SIMPLERPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

protected:
	/**************************
	 * Inventory Functions 
	 **************************/
	UFUNCTION(BlueprintCallable)
	void OnPageSelected(int32 PageIndex);

	UFUNCTION(BlueprintCallable)
	void OnCurrentPageSort();

	//UFUNCTION(BlueprintCallable)
	//void OnItemSelected();

	//UFUNCTION(BlueprintCallable)
	//void OnItemDragAndDropped();



protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	int32 PageWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<UUserWidget> SlotWidgetClass;

	//UPROPERTY()
	//TArray<TObjectPtr<class UBorder>> Slots;

	UPROPERTY(meta = (BindWidget), EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;

};
