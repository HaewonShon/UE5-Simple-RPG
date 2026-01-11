// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestDisplayWidget.generated.h"

/**
 *  Widget holding quest status widgets
 */
UCLASS()
class SIMPLERPG_API UQuestDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();

	void RegisterQuest(const class UQuestData* Quest);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UQuestStatusWidget> StatusWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> StatusWidgetSlot;
};
