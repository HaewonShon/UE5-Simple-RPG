// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestOfferButtonWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UQuestOfferButtonWidget::SetContent(class UTexture2D* Icon, FText QuestTitle, int32 ResponseIndex)
{
	IconImage->SetBrushFromTexture(Icon);
	TitleText->SetText(QuestTitle);
	SelfResponseIndex = ResponseIndex;

	OfferButton->OnClicked.AddDynamic(this, &UQuestOfferButtonWidget::OnButtonClicked);
}

void UQuestOfferButtonWidget::OnButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("%s Button clicked"), *(TitleText->GetText().ToString()));
	OnResponseSelected.ExecuteIfBound(SelfResponseIndex);
}