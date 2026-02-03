// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PortraitDisplayWidget.generated.h"

/**
 *		Character portrait & level display widget
 */
UCLASS()
class SIMPLERPG_API UPortraitDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void OnLevelChanged(int32 NewLevel);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Portrait;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UTextBlock> LevelText;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> RenderTarget;
};
