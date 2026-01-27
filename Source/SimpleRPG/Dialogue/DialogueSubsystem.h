// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueData.h"
#include "DialogueSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDialogueBegin)
DECLARE_MULTICAST_DELEGATE(FOnDialogueEnd)
DECLARE_DELEGATE_OneParam(FOnDialogueUpdate, FDialogueInfo)

/*
*   Dialogue response from UI 
*/
UENUM(Blueprintable)
enum EDialogueResponseType : int8
{
	Continue,
	Exit,
	QuestSelect,
	QuestAccept,
	QuestDecline,
};


USTRUCT()
struct FDialogueResponse
{
	GENERATED_BODY()
	
	EDialogueResponseType Type;
	FText QuestTitle;
	FPrimaryAssetId QuestId; // temporary paylaod for quest selection
	class UTexture2D* QuestStatusTexture;
};

/*
*   struct for passing dialogue to UI
*/
USTRUCT()
struct FDialogueInfo
{
	GENERATED_BODY()

	FText NPCName;
	FText DialogueText;
	TArray<FDialogueResponse> Responses;
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
	void BeginDialogue(class ANPCCharacter* NPC, class ASimpleRPGPlayerState* PS);

	UFUNCTION()
	void OnDialogueResponses(FDialogueResponse Response);

	FDialogueInfo RequestCurrentDialogueInfo();

	FOnDialogueBegin OnDialogueBegin;
	FOnDialogueEnd OnDialogueEnd;
	FOnDialogueUpdate OnDialogueUpdate;

protected:
	void UpdateDialogueNode(int NextNodeIndex);
	FDialogueInfo BuildDialogueWithCurrentNode();
	EQuestDialogueContext ResolveQuestDialogueContext(FPrimaryAssetId QuestId);
	
	TWeakObjectPtr<class UDialogueData> CurrentDialogueData;
	int32 CurrentNodeIndex;

	TWeakObjectPtr<class ASimpleRPGPlayerState> PlayerStateRef;
	TWeakObjectPtr<AActor> InteractingTargetRef;

	// TEMP ICONS
	UPROPERTY()
	TObjectPtr<class UTexture2D> QuestAvailableIcon;

	UPROPERTY()
	TObjectPtr<class UTexture2D> QuestInProgressIcon; 
};

