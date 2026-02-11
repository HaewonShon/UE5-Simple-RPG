// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "../../Character/SimpleRPGPlayerController.h"
#include "../../Dialogue/DialogueData.h"
#include "Components/TextBlock.h"
#include "../../Dialogue/DialogueSubsystem.h"
#include "Components/VerticalBox.h"
#include "ActionButtonWidget.h"
#include "Components/Button.h"

void UDialogueWidget::UpdateDialogue(FDialogueInfo DialogueInfo)
{
	// clear prev buttons
	ActionButtonSlot->ClearChildren();
	QuestAcceptButton->SetVisibility(ESlateVisibility::Collapsed);
	QuestDeclineButton->SetVisibility(ESlateVisibility::Collapsed);

	NPCName->SetText(DialogueInfo.NPCName);
	DialogueText->SetText(DialogueInfo.DialogueText);

	for (const FActionInfo& Action : DialogueInfo.Actions)
	{
		switch (Action.Type)
		{
		case EActionType::QuestAccept:
			QuestAcceptButton->SetVisibility(ESlateVisibility::Visible);
			QuestAcceptButton->SetContent(Action);
			break;
		case EActionType::QuestDecline:
			QuestDeclineButton->SetVisibility(ESlateVisibility::Visible);
			QuestDeclineButton->SetContent(Action);
			break;
		default:
		{
			UActionButtonWidget* ActionButton = CreateWidget<UActionButtonWidget>(this, ActionButtonWidgetClass);
			if (ActionButton)
			{
				ActionButton->SetContent(Action);
				ActionButtonSlot->AddChildToVerticalBox(ActionButton);
			}
			break;
		}}
	}
}

void UDialogueWidget::NativeConstruct()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueUpdate.BindUObject(this, &UDialogueWidget::UpdateDialogue);
		UpdateDialogue(Subsystem->RequestCurrentDialogueInfo());
	}
	QuestAcceptButton->SetVisibility(ESlateVisibility::Collapsed);
	QuestDeclineButton->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UDialogueWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (Reply.NativeReply.IsEventHandled() || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Reply.NativeReply;
	}

	SetNextPage();
	return FReply::Handled();
}

void UDialogueWidget::SetNextPage()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->SetNextPage();
	}
}