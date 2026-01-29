// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "../../Character/SimpleRPGPlayerController.h"
#include "../../Dialogue/DialogueData.h"
#include "Components/TextBlock.h"
#include "../../Dialogue/DialogueSubsystem.h"
#include "Components/VerticalBox.h"
#include "../Quest/QuestOfferButtonWidget.h"
#include "Components/Button.h"

void UDialogueWidget::IntializeDialogue(const UDialogueData* Dialogue)
{
	//if (!Dialogue || Dialogue->Dialogue.Num() == 0)
	//{
	//	return;
	//}

	/*UE_LOG(LogTemp, Log, TEXT("Dialogue init"));
	DialogueData = Dialogue;

	NPCName->SetText(DialogueData->NPCName);
	CurrentPageIndex = 0;*/
	//DialogueText->SetText(DialogueData->Dialogue[CurrentPageIndex]);
}

void UDialogueWidget::UpdateDialogue(FDialogueInfo DialogueInfo)
{
	// clear prev buttons
	QuestOfferButtonSlot->ClearChildren();
	QuestAcceptButton->SetVisibility(ESlateVisibility::Collapsed);
	QuestDeclineButton->SetVisibility(ESlateVisibility::Collapsed);

	NPCName->SetText(DialogueInfo.NPCName);
	DialogueText->SetText(DialogueInfo.DialogueText);
	CachedResponses = DialogueInfo.Responses;

	int32 ResponseIndex = 0;
	for (const FDialogueResponse& Response : DialogueInfo.Responses)
	{
		switch (Response.Type)
		{
		case EDialogueResponseType::QuestSelect: // create quest offer
		{
			UQuestOfferButtonWidget* QuestOfferButton = CreateWidget<UQuestOfferButtonWidget>(this, QuestOfferButtonClass);
			if (QuestOfferButton)
			{
				QuestOfferButton->SetContent(Response.QuestStatusTexture, Response.QuestTitle, ResponseIndex++);
				QuestOfferButton->OnResponseSelected.BindUObject(this, &UDialogueWidget::OnRespond);
				QuestOfferButtonSlot->AddChildToVerticalBox(QuestOfferButton);
				UE_LOG(LogTemp, Log, TEXT("Quest button added"));
			}
		}
			break;
		case EDialogueResponseType::QuestAccept:
			QuestAcceptButton->SetVisibility(ESlateVisibility::Visible);
			AcceptResponseIndex = ResponseIndex++;
			break;
		case EDialogueResponseType::QuestDecline:
			QuestDeclineButton->SetVisibility(ESlateVisibility::Visible);
			DeclineResponseIndex = ResponseIndex++;
			break;
		}
	}
}

void UDialogueWidget::NativeConstruct()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueUpdate.BindUObject(this, &UDialogueWidget::UpdateDialogue);
		UpdateDialogue(Subsystem->RequestCurrentDialogueInfo());
	}

	QuestAcceptButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnQuestAcceptButtonClicked);
	QuestDeclineButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnQuestDeclineButtonClicked);
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

void UDialogueWidget::OnRespond(int32 ResponseIndex)
{
	if (ResponseIndex < 0 || ResponseIndex >= CachedResponses.Num())
	{
		return;
	}

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueResponse(CachedResponses[ResponseIndex]);
	}
}

void UDialogueWidget::SetNextPage()
{
	FDialogueResponse Response;
	Response.Type = EDialogueResponseType::Continue;

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueResponse(Response);
	}
}

void UDialogueWidget::OnQuestAcceptButtonClicked()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueResponse(CachedResponses[AcceptResponseIndex]);
	}
}

void UDialogueWidget::OnQuestDeclineButtonClicked()
{
	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueResponse(CachedResponses[DeclineResponseIndex]);
	}
}
