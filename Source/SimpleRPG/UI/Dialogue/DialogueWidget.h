// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

DECLARE_DELEGATE(FOnDialogueFinished)

/**
 *		Widget for displaying dialogue with NPC
 */
UCLASS()
class SIMPLERPG_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void IntializeDialogue(const class UDialogueData* Dialogue);
	void UpdateDialogue(struct FDialogueInfo DialogueInfo);
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void OnRespond(int32 ResponseIndex);

	UFUNCTION(BlueprintCallable)
	void SetNextPage();

	// temp for accept/decline buttons
	UFUNCTION()
	void OnQuestAcceptButtonClicked();

	UFUNCTION()
	void OnQuestDeclineButtonClicked();

	/* Bind Widgets */
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> NPCName;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> DialogueText;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UVerticalBox> QuestOfferButtonSlot;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> QuestAcceptButton;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> QuestDeclineButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UQuestOfferButtonWidget> QuestOfferButtonClass;

	UPROPERTY()
	TArray<struct FDialogueResponse> CachedResponses;

	int32 AcceptResponseIndex;
	int32 DeclineResponseIndex;
};
