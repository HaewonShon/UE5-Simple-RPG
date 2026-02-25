// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/MessageBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UMessageBox::SetMessage(const FText& Message)
{
	MessageTextBlock->SetText(Message);
}

void UMessageBox::OnConfirmClicked()
{
	CloseWidget();
}

void UMessageBox::NativeConstruct()
{
	ConfirmButton->OnClicked.AddDynamic(this, &UMessageBox::OnConfirmClicked);
}