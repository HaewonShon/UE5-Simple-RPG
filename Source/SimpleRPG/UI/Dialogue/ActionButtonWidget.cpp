// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionButtonWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "../../Dialogue/DialogueSubsystem.h"

void UActionButtonWidget::SetContent(const FActionInfo& Action)
{
	CachedAction = Action;
	TitleText->SetText(Action.DisplayName);

	if (Action.Icon)
	{
		IconImage->SetBrushFromTexture(Action.Icon);
	}

	ActionButton->OnClicked.AddDynamic(this, &UActionButtonWidget::OnButtonClicked);
}

void UActionButtonWidget::OnButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("%s Button clicked"), *(TitleText->GetText().ToString()));

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->SetContextOwner(CachedAction.ContextOwner);
	}
	CachedAction.OnActionExecuted.ExecuteIfBound();
}