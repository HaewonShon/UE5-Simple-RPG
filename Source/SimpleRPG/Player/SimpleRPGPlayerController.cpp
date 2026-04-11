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
#include "Interaction/Shop/ShopComponent.h"
#include "Interaction/Enhancement/UI/EnhancementWidget.h"
#include "Interaction/Enhancement/EnhancementComponent.h"

#include "World/NPCCharacter.h"
#include "Shared/UI/RootWidget.h"
#include "Shared/UI/UISubsystem.h"
#include "Components/InventoryComponent.h"
#include "UI/Inventory/InventoryWidget.h"

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
		EIC->BindAction(CharacterInfoToggleAction, ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::ToggleCharacterInfo);
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
	bIsCharacterInfoOn = false;
	OnDialogueRequested.AddUObject(this, &ASimpleRPGPlayerController::BeginDialogue);
}

void ASimpleRPGPlayerController::AddPitchInput(float Val)
{
	Super::AddPitchInput(Val);
}

void ASimpleRPGPlayerController::ToggleInventory()
{
	UISubsystem->ToggleInventory();
	bIsInvenetoryOn = !bIsInvenetoryOn;

	if (bIsInvenetoryOn || bIsCharacterInfoOn)
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

void ASimpleRPGPlayerController::ToggleCharacterInfo()
{
	UISubsystem->ToggleCharacterInfo();
	bIsCharacterInfoOn = !bIsCharacterInfoOn;

	if (bIsInvenetoryOn || bIsCharacterInfoOn)
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
	// Build UI, camera and begin dialogue
	DialogueDisplayWidget = Cast<UDialogueWidget>(UISubsystem->AddWidgetToLayer(EWidgetLayer::Dialogue, DialogueDisplayWidgetClass, true));
	if (!DialogueDisplayWidget)
	{
		UE_LOG(LogPlayerController, Error, TEXT("BeginDialogue Failed to create widget"));
		return;
	}

	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->BeginDefaultDialogue(NPC, GetPlayerState<ASimpleRPGPlayerState>());
	}
	BuildDialogueCamera(NPC);

	// Input setting
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(FInputModeUIOnly());

	// prevent input from player
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	InputSystemRef->AddMappingContext(DialogueInputMapping, 10);

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
		DialogueDisplayWidget->CloseWidget();
		DialogueDisplayWidget = nullptr;
	}
}

void ASimpleRPGPlayerController::OpenShop(class UShopComponent* ShopComponent)
{
	// create shop ui widget
	ShopWidget = Cast<UShopWidget>(UISubsystem->AddWidgetToLayer(EWidgetLayer::Menu, ShopWidgetClass, false, false));
	if (!ShopWidget)
	{
		UE_LOG(LogPlayerController, Error, TEXT("OpenShop Failed to create widget"));
		return;
	}

	ShopWidget->InitializeShop(ShopComponent);
	ShopWidget->SetDescriptionWidgetRef(UISubsystem->GetItemDescriptionWidget());
	ShopWidget->OnWidgetClosed.AddUObject(this, &ASimpleRPGPlayerController::CloseShop);
	
	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		PS->GetInventoryComponent()->SetShopMode(ShopComponent);
		ShopComponent->SetPlayerStateRef(PS);
	}

	// open inventory UI
	if (!bIsInvenetoryOn)
	{
		ToggleInventory();
	}
	// TODO : Set inventory widget position
}

void ASimpleRPGPlayerController::CloseShop()
{
	ShopWidget = nullptr;
	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		PS->GetInventoryComponent()->SetNormalMode();
	}

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->SetNextPage();
	}
}

void ASimpleRPGPlayerController::OpenEnhancement(class UEnhancementComponent* EnhancementComponent)
{
	// create enhancment UI
	EnhancementWidget = Cast<UEnhancementWidget>(UISubsystem->AddWidgetToLayer(EWidgetLayer::Menu, EnhancementWidgetClass, false, false));
	if (EnhancementWidget)
	{
		// TODO: register widget-related if required
		EnhancementWidget->SetDescriptionWidgetRef(UISubsystem->GetItemDescriptionWidget());
		EnhancementWidget->OnWidgetClosed.AddUObject(this, &ASimpleRPGPlayerController::CloseEnhancement);
	}
	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		PS->GetInventoryComponent()->SetEnhanceMode();
	}

	// open inventory UI
	if (!bIsInvenetoryOn)
	{
		ToggleInventory();
	}
	// TODO : Set inventory widget position

	if (!bIsCharacterInfoOn)
	{
		ToggleCharacterInfo();
	}
	// TODO : Set Character info widget position
}

void ASimpleRPGPlayerController::CloseEnhancement()
{
	if (EnhancementWidget)
	{
		EnhancementWidget = nullptr;
	}

	if (ASimpleRPGPlayerState* PS = GetPlayerState<ASimpleRPGPlayerState>())
	{
		PS->GetInventoryComponent()->SetNormalMode();
	}

	if (UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Subsystem->SetNextPage();
	}
}

void ASimpleRPGPlayerController::ConstructUI()
{
	check(RootWidgetClass);
	UISubsystem = GetLocalPlayer()->GetSubsystem<UUISubsystem>();
	if (UISubsystem.IsValid())
	{
		UISubsystem->InitializeRootWidget(this, RootWidgetClass);
		UInventoryWidget* InventoryWidget = UISubsystem->GetInventoryWidget();
		InventoryWidget->OnWidgetClosed.AddUObject(this, &ASimpleRPGPlayerController::ToggleInventory);

	}
	else
	{
		UE_LOG(LogPlayerController, Warning, TEXT("UI Susbystem not set"));
	}
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