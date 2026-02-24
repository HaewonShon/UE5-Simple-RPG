// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "DialogueWidget.generated.h"

DECLARE_DELEGATE(FOnDialogueFinished)

/**
 *		Widget for displaying dialogue with NPC
 */
UCLASS()
class SIMPLERPG_API UDialogueWidget : public USessionWidget
{
	GENERATED_BODY()

public:
	void UpdateDialogue(struct FDialogueInfo DialogueInfo);
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable)
	void SetNextPage();

	/* Bind Widgets */
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> NPCName;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> DialogueText;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UVerticalBox> ActionButtonSlot;


	/***** Quest Buttons *****/
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UActionButtonWidget> QuestAcceptButton;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UActionButtonWidget> QuestDeclineButton;

	/***** Class for button build *****/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UActionButtonWidget> ActionButtonWidgetClass;
};
