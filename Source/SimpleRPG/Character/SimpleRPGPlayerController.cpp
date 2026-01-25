// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerController.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../UI/SimpleRPGHUDWidget.h"
#include "../Dialogue/DialogueCameraActor.h"
#include "../UI/DialogueWidget.h"
#include "../Dialogue/DialogueSubsystem.h"

void ASimpleRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);
	InputSystemRef = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystemRef.IsValid())

		// Input setup for UI
		if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent))
		{
			EIC->BindAction(InventoryToggleAction, ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::ToggleInventory);
			InputSystemRef->AddMappingContext(UIMapping, 1);

#if !UE_BUILD_SHIPPING
			ensure(CheatAction.Num() >= 4);
			EIC->BindAction(CheatAction[0], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction1);
			EIC->BindAction(CheatAction[1], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction2);
			EIC->BindAction(CheatAction[2], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction3);
			EIC->BindAction(CheatAction[3], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction4);
			InputSystemRef->AddMappingContext(CheatMapping, 0);
#endif

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController requires EnhancedInputComponent"));
		}

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USimpleRPGHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HUDWidget in PlayerController not registered."));
	}

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->OnDialogueEnd.AddUObject(this, &ASimpleRPGPlayerController::FinishDialogue);
	}

	bIsInvenetoryOn = false;
	OnDialogueRequested.AddUObject(this, &ASimpleRPGPlayerController::BeginDialogue);
}

void ASimpleRPGPlayerController::AddPitchInput(float Val)
{
    Super::AddPitchInput(Val);
}

void ASimpleRPGPlayerController::ToggleInventory()
{
	if (HUDWidget)
	{
		HUDWidget->ToggleInventory();
		bIsInvenetoryOn = !bIsInvenetoryOn;

		if(bIsInvenetoryOn)
		{
			bShowMouseCursor = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetWidgetToFocus(HUDWidget->GetInventoryWidget()->TakeWidget());

			SetInputMode(InputMode);
		}
		else
		{
			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
	}
}

void ASimpleRPGPlayerController::BeginDialogue(AActor* NPC)
{
	// Input setting
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(HUDWidget->TakeWidget());
	 
	SetInputMode(FInputModeUIOnly());

	// prevent input from player
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	InputSystemRef->AddMappingContext(DialogueInputMapping, 10);

	BuildDialogueCamera(NPC);
}

void ASimpleRPGPlayerController::FinishDialogue()
{
	// Input setting
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	// Controller setting
	constexpr float DIALOGUE_CAM_BLEND_TIME = 0.5f;
	SetViewTargetWithBlend(GetCharacter(), DIALOGUE_CAM_BLEND_TIME);

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	InputSystemRef->RemoveMappingContext(DialogueInputMapping);
	ClearDialogueCamera();
}

void ASimpleRPGPlayerController::BuildDialogueCamera(AActor* NPC)
{// Controller setting
	constexpr float DIALOGUE_CAM_BLEND_TIME = 0.5f;

	DialogueCameraActor = GetWorld()->SpawnActor<ADialogueCameraActor>(DialogueCameraActorClass);
	DialogueCameraActor->SetupCameraTransform(GetCharacter()->GetActorLocation(), NPC->GetActorLocation());
	SetViewTargetWithBlend(DialogueCameraActor.Get(), DIALOGUE_CAM_BLEND_TIME);

	// Create Dialogue Widget
	DialogueDisplayWidget = CreateWidget<UDialogueWidget>(this, DialogueDisplayWidgetClass.Get());
	if (DialogueDisplayWidget)
	{
		constexpr int32 DIALOGUE_ZORDER = 1000;
		DialogueDisplayWidget->AddToViewport(DIALOGUE_ZORDER);
		//DialogueDisplayWidget->IntializeDialogue(NPC, Dialogue);
	}

}
void ASimpleRPGPlayerController::ClearDialogueCamera()
{
	if (DialogueCameraActor)
	{
		constexpr float DIALOGUE_CAM_BLEND_TIME = 0.5f;
		DialogueCameraActor->SetLifeSpan(DIALOGUE_CAM_BLEND_TIME);
		DialogueCameraActor = nullptr;
	}

	if (DialogueDisplayWidget)
	{
		DialogueDisplayWidget->RemoveFromParent();
		DialogueDisplayWidget = nullptr;
	}
}

/*
*	Cheat input
*/
#if !UE_BUILD_SHIPPING
#include "../ItemTestCheatManager.h"
#include "../Quest/QuestTestCheatManager.h"

void ASimpleRPGPlayerController::CheatFunction1()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(0);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(0);
	}
}

void ASimpleRPGPlayerController::CheatFunction2()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(1);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(1);
	}
}

void ASimpleRPGPlayerController::CheatFunction3()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(2);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(2);
	}
}

void ASimpleRPGPlayerController::CheatFunction4()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(3);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(3);
	}
}
#endif