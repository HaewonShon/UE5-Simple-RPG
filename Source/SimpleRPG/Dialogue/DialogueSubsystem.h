// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueData.h"
#include "../Quest/QuestManagerSubsystem.h"
#include "GameSystem/Interactions/ActionProvider.h"
#include "DialogueSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDialogueBegin)
DECLARE_MULTICAST_DELEGATE(FOnDialogueEnd)
DECLARE_DELEGATE_OneParam(FOnDialogueUpdate, FDialogueInfo)

/*
*   struct for passing dialogue to UI
*/
USTRUCT()
struct FDialogueInfo
{
	GENERATED_BODY()

	FText NPCName;
	FText DialogueText;
	TArray<FActionInfo> Actions;
};

/**
 *	Subsystem for dialogue
 *  Manage dialogue request, dialogue flow and response from UI
 */
UCLASS()
class SIMPLERPG_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	UFUNCTION()
	void BeginDefaultDialogue(class ANPCCharacter* NPC, class ASimpleRPGPlayerState* PS);

	UFUNCTION()
	void BeginDialogue(UDialogueData* Dialogue, int32 DialogueBeginNode = -1);

	void SetContextOwner(const UActorComponent* Owner);
	void SetNextPage();

	FDialogueInfo RequestCurrentDialogueInfo();

	FOnDialogueBegin OnDialogueBegin;
	FOnDialogueEnd OnDialogueEnd;
	FOnDialogueUpdate OnDialogueUpdate;

protected:
	void UpdateDialogueNode(int NextNodeIndex);

	FDialogueInfo BuildDialogueWithCurrentNode();
	
	TWeakObjectPtr<class UDialogueData> CurrentDialogueData;
	int32 CurrentNodeIndex;

	TWeakObjectPtr<class ASimpleRPGPlayerState> PlayerStateRef;
	TWeakObjectPtr<AActor> InteractingTargetRef;
	TWeakObjectPtr<const UActorComponent> ContextOwnerRef;
	FPrimaryAssetId CurrentQuestId;
};

