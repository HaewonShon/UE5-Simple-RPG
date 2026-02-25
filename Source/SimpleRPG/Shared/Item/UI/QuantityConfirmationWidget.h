// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "QuantityConfirmationWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnQuantityConfirmed, int32);

/**
 *	A Widget to get a quantity of an item from player
 */
UCLASS()
class SIMPLERPG_API UQuantityConfirmationWidget : public USessionWidget
{
	GENERATED_BODY()

public:
	void DisplayMessage(FText Message);

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

	UFUNCTION()
	void OnTextChanged(const FText& Text);

	FOnQuantityConfirmed OnQuantityConfirmed;

protected:
	virtual void NativeConstruct() override;
	int32 GetQuantity() const;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UEditableText> QuantityText;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> ConfirmButton;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> CancelButton;

	UPROPERTY(EditDefaultsOnly, Category = "Confirmation Widget")
	TSubclassOf<class UMessageBox> MessageBoxClass;

	FText LatestValidText;
};
