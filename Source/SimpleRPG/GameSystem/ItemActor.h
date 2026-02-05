// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item/ItemData.h"
#include "DropActor.h"
#include "ItemActor.generated.h"

UCLASS()
class SIMPLERPG_API AItemActor : public ADropActor
{
	GENERATED_BODY()

public:
	AItemActor();
	void SetItem(FItemInstance Item);

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor EquipmentColor;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor ConsumableColor;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor MaterialColor;

	FItemInstance ItemInstance;
};