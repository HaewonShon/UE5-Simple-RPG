// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Item/UI/QuantityConfirmationWidget.h"
#include "Shared/UI/MessageBox.h"
#include "Components/Button.h"
#include "Components/EditableText.h"


void UQuantityConfirmationWidget::DisplayMessage(FText Message)
{
	// setup confirm box and display message
	UMessageBox* MessageBox = CreateWidget<UMessageBox>(this, MessageBoxClass.Get());
	if (MessageBox)
	{
		MessageBox->SetMessage(Message);
	}
}

void UQuantityConfirmationWidget::OnConfirmClicked()
{
	OnQuantityConfirmed.ExecuteIfBound(GetQuantity());
	CloseWidget();
}

void UQuantityConfirmationWidget::OnCancelClicked()
{
	CloseWidget();
}

void UQuantityConfirmationWidget::OnTextChanged(const FText& Text)
{
	if (Text.IsNumeric())
	{
		LatestValidText = Text;
	}
	else
	{
		QuantityText->SetText(LatestValidText);
	}
}

void UQuantityConfirmationWidget::NativeConstruct()
{
	LatestValidText = FText::AsNumber(0);
	QuantityText->SetText(LatestValidText);

	ConfirmButton->OnClicked.AddDynamic(this, &UQuantityConfirmationWidget::OnConfirmClicked);
	CancelButton->OnClicked.AddDynamic(this, &UQuantityConfirmationWidget::OnCancelClicked);
	QuantityText->OnTextChanged.AddDynamic(this, &UQuantityConfirmationWidget::OnTextChanged);
}

int32 UQuantityConfirmationWidget::GetQuantity() const
{	
	int32 Quantity = FCString::Atoi(*(LatestValidText.ToString()));
	return Quantity;
}
