// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestOfferButtonWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnResponseSelected, int32 ResponseIndex)

/**
 *  A button for quest selection in dialogue interface
 */
UCLASS()
class SIMPLERPG_API UQuestOfferButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    void SetContent(class UTexture2D* Icon, FText QuestTitle, int32 ResponseIndex);

    UFUNCTION()
    void OnButtonClicked();

    FOnResponseSelected OnResponseSelected;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UButton> OfferButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> IconImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> TitleText;

    int32 SelfResponseIndex;
};
