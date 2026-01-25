// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include "../Character/SimpleRPGPlayerController.h"
#include "../Dialogue/DialogueData.h"
#include "Components/TextBlock.h"
#include "../Dialogue/DialogueSubsystem.h"

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
	NPCName->SetText(DialogueInfo.NPCName);
	DialogueText->SetText(DialogueInfo.DialogueText);
}

void UDialogueWidget::NativeConstruct()
{
	if (ASimpleRPGPlayerController* PC = Cast<ASimpleRPGPlayerController>(GetOwningPlayer()))
	{
		UE_LOG(LogTemp, Log, TEXT("Dialogue init2"));
		//PC->OnDialogueRequested.AddUObject(this, &UDialogueWidget::IntializeDialogue);
		//OnDialogueFinished.BindUObject(PC, &ASimpleRPGPlayerController::FinishDialogue);
	}

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueUpdate.BindUObject(this, &UDialogueWidget::UpdateDialogue);
		UpdateDialogue(Subsystem->RequestCurrentDialogueInfo());
	}
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
	FDialogueResponse Response;
	Response.Type = EDialogueResponseType::Continue;

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueResponses(Response);
	}

	/*if (CurrentPageIndex < DialogueData->Dialogue.Num() - 1)
	{
		DialogueText->SetText(DialogueData->Dialogue[++CurrentPageIndex]);
	}
	else
	{
		OnDialogueFinished.ExecuteIfBound();
	}*/
}
