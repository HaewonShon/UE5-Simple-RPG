// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SimpleRPGPlayerController.generated.h"

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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class USimpleRPGHUDWidget> HUDWidgetClass;

	TObjectPtr<class USimpleRPGHUDWidget>  HUDWidget;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* UIMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* InventoryToggleAction;
};
