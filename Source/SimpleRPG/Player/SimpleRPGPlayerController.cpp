// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerController.h"
#include "SimpleRPGPlayerState.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Interaction/Dialogue/DialogueCameraActor.h"
#include "Interaction/Dialogue/UI/DialogueWidget.h"
#include "Interaction/Dialogue/DialogueSubsystem.h"
#include "Interaction/Shop/UI/ShopWidget.h"
#include "World/NPCCharacter.h"
#include "Shared/UI/RootWidget.h"

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

	ConstructUI();

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
	RootWidget->ToggleInventory();
	bIsInvenetoryOn = !bIsInvenetoryOn;
	if (bIsInvenetoryOn)
	{
		bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//InputMode.SetWidgetToFocus(InventoryWidget->GetCachedWidget());

		SetInputMode(InputMode);
	}
	else
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void ASimpleRPGPlayerController::BeginDialogue(ANPCCharacter* NPC)
{
	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->BeginDefaultDialogue(NPC, GetPlayerState<ASimpleRPGPlayerState>());
	}

	// Input setting
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//InputMode.SetWidgetToFocus(HUDWidget->TakeWidget());

	SetInputMode(FInputModeUIOnly());

	// prevent input from player
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	InputSystemRef->AddMappingContext(DialogueInputMapping, 10);

	BuildDialogueCamera(NPC);

	// Create Dialogue Widget
	DialogueDisplayWidget = Cast<UDialogueWidget>(RootWidget->AddWidgetToLayer(EWidgetLayer::Dialogue, DialogueDisplayWidgetClass, true));
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

	if (DialogueDisplayWidget)
	{
		DialogueDisplayWidget->RemoveFromParent();
		DialogueDisplayWidget = nullptr;
	}
}

void ASimpleRPGPlayerController::OpenShop(class UShopComponent* ShopComponent)
{
	// create shop ui widget
	ShopWidget = CreateWidget<UShopWidget>(this, ShopWidgetClass.Get());
	if (ShopWidget)
	{
		constexpr int32 SHOP_ZORDER = 2000;
		ShopWidget->AddToViewport(SHOP_ZORDER);
		ShopWidget->InitializeShop(ShopComponent);
		// TODO : dSet Description widget
		//ShopWidget->SetDescriptionWidgetRef(HUDWidget->GetItemDescriptionWidget());
	}

	// open inventory widget
	ToggleInventory();
	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		//PS->GetInventoryComponent()->SetShopMode();
	}

	// ui widget - put shop component 주입, 주입된 shop component에 bind
	// 구매를 ACTION 어떻게 할까???????

}

void ASimpleRPGPlayerController::CloseShop()
{
	ShopWidget->RemoveFromParent();
	ToggleInventory();
	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		//PS->GetInventoryComponent()->SetNormalMode();
	}
}

void ASimpleRPGPlayerController::ConstructUI()
{
	check(RootWidgetClass);

	RootWidget = CreateWidget<URootWidget>(this, RootWidgetClass.Get());
	RootWidget->AddToViewport();
}

void ASimpleRPGPlayerController::BuildDialogueCamera(ANPCCharacter* NPC)
{
	constexpr float DIALOGUE_CAM_BLEND_TIME = 0.5f;

	DialogueCameraActor = GetWorld()->SpawnActor<ADialogueCameraActor>(DialogueCameraActorClass);
	DialogueCameraActor->SetupCameraTransform(GetCharacter()->GetActorLocation(), NPC->GetActorLocation());
	SetViewTargetWithBlend(DialogueCameraActor.Get(), DIALOGUE_CAM_BLEND_TIME);
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
#include "ItemTestCheatManager.h"
#include "Interaction/Quest/QuestTestCheatManager.h"

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