// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Item/ItemData.h"
#include "ItemDescriptionWidget.generated.h"
/**
 *	widget for displaying item description, include name / type / detail
 */
UCLASS(Blueprintable)
class SIMPLERPG_API UItemDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDescription(const FItemDescription& Description);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DisplayName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> DisplayIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Type;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DetailedText;
};
