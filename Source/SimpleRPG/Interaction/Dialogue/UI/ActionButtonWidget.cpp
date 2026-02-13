// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionButtonWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "../DialogueSubsystem.h"

void UActionButtonWidget::SetContent(const FActionInfo& Action)
{
	CachedAction = Action;
	TitleText->SetText(Action.DisplayName);

	if (Action.Icon)
	{
		IconImage->SetBrushFromTexture(Action.Icon);
	}
}

void UActionButtonWidget::OnButtonClicked()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->SetContextOwner(CachedAction.ContextOwner);
	}
	CachedAction.OnActionExecuted.ExecuteIfBound();
}

void UActionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButton->OnClicked.AddDynamic(this, &UActionButtonWidget::OnButtonClicked);
}
