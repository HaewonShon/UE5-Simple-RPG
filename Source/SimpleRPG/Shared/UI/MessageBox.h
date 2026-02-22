// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBox.generated.h"

/**
 *	 Text box to display message
 */
UCLASS()
class SIMPLERPG_API UMessageBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMessage(FText Message);

	UFUNCTION()
	void OnConfirmClicked();
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> MessageTextBlock;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> ConfirmButton;
};
