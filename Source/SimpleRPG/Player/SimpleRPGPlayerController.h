// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SimpleRPGPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialogueRequested, class ANPCCharacter*); // target NPC
DECLARE_DELEGATE_OneParam(FOnShopOpenRequest, class UShopComponent*);

/**
 *	Player Controller Class for registering HUD
 */
UCLASS()
class SIMPLERPG_API ASimpleRPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void AddPitchInput(float Val) override;

	void ToggleInventory();
	void ToggleCharacterInfo();

	FOnDialogueRequested OnDialogueRequested;
	FOnShopOpenRequest OnShopOpenRequested;

	UFUNCTION()
	void BeginDialogue(class ANPCCharacter* NPC);

	UFUNCTION()
	void FinishDialogue();

	UFUNCTION()
	void OpenShop(class UShopComponent* ShopComponent);

	UFUNCTION()
	void CloseShop();

protected:
	void ConstructUI();

	TWeakObjectPtr<class UUISubsystem> UISubsystem;

	/*************************************************
	*	UI & Input
	*************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class URootWidget> RootWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DialogueInputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* UIMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InventoryToggleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* CharacterInfoToggleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* CheatMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<class UInputAction*> CheatAction;

	bool bIsInvenetoryOn;
	bool bIsCharacterInfoOn;

	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSystemRef;

	/*************************************************
	*	Dialogue System
	*************************************************/
	void BuildDialogueCamera(class ANPCCharacter* NPC);
	void ClearDialogueCamera();

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TSubclassOf<class ADialogueCameraActor> DialogueCameraActorClass;

	UPROPERTY()
	TObjectPtr<class ADialogueCameraActor> DialogueCameraActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialogue")
	TSubclassOf<class UDialogueWidget> DialogueDisplayWidgetClass;

	UPROPERTY()
	TObjectPtr<class UDialogueWidget> DialogueDisplayWidget;

	/*************************************************
	*	Shop UI
	*************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<class UShopWidget> ShopWidgetClass;

	UPROPERTY()
	TObjectPtr<class UShopWidget> ShopWidget;


#if !UE_BUILD_SHIPPING
	void CheatFunction1();
	void CheatFunction2();
	void CheatFunction3();
	void CheatFunction4();
#endif
};
