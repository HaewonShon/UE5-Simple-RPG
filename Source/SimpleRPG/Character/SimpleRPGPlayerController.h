// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SimpleRPGPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDialogueRequested, const class UDialogueData*)

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

	FOnDialogueRequested OnDialogueRequested;
	void BeginDialogue(const class UDialogueData* Dialogue);
	void FinishDialogue();

protected:
	/*
	*	UI & Input
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class USimpleRPGHUDWidget> HUDWidgetClass;

	TObjectPtr<class USimpleRPGHUDWidget>  HUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* GameInputMaapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* UIMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InventoryToggleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* CheatMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<class UInputAction*> CheatAction;

	bool bIsInvenetoryOn;

#if !UE_BUILD_SHIPPING
	void CheatFunction1();
	void CheatFunction2();
	void CheatFunction3();
	void CheatFunction4();
#endif
};
