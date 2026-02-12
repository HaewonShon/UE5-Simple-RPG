// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Core/ActionProvider.h"
#include "ActionButtonWidget.generated.h"

/**
 *  A button for quest selection in dialogue interface
 */
UCLASS()
class SIMPLERPG_API UActionButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    void SetContent(const FActionInfo& Action);

    UFUNCTION()
    void OnButtonClicked();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> ActionButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> IconImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> TitleText;

    FActionInfo CachedAction;
};
