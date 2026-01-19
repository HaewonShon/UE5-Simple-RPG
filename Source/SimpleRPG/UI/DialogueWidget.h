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
	void IntializeDialogue(AActor* NPC, const class UDialogueData* Dialogue);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable)
	void SetNextPage();

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> NPCName;

	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UTextBlock> DialogueText;

	TWeakObjectPtr<const class UDialogueData> DialogueData;
	int32 CurrentPageIndex;

	FOnDialogueFinished OnDialogueFinished;
};
