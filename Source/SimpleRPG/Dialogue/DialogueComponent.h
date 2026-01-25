// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void BeginDialogue(class ASimpleRPGPlayerController* Controller);

	class UDialogueData* GetDefaultDialogue() const;

	class UDialogueData* GetQuestDialogue(FPrimaryAssetId QuestId) const;

protected:
	UPROPERTY(EditAnywhere)
	class UDialogueData* DefaultDialogue;

	UPROPERTY(EditAnywhere)
	TMap<FPrimaryAssetId, class UDialogueData*> QuestDialogues;
};
