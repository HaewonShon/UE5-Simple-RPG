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

	UFUNCTION(BlueprintCallable)
	void SetNextPage();

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> NPCName;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> DialogueText;
};
