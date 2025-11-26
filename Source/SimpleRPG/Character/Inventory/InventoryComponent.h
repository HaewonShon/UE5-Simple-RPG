// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.h"
#include "../../Item/ItemData.h"
#include "InventoryComponent.generated.h"

// Define Log Inventory for Inventory-specific logs
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All)

DECLARE_DELEGATE_OneParam(FInventoryContentChangedDelegate, EInventoryCategory)

UCLASS(ClassGroup = (SimpleRPG), meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void BeginPlay() override;

	bool AddItem(FItemInstance ItemInstance);
	//void RemoveItem(UItemData* Item);
	//bool EquipItem(UItemData* Item);
	//bool UnequipItem(UItemData* Item);
	//bool UseItem(UItemData* Item);

	const FInventoryPage& GetPage(EInventoryCategory PageCategory) const;

	FInventoryContentChangedDelegate OnInventoryContentChanged;

protected:
	UPROPERTY()
	TMap<EInventoryCategory, FInventoryPage> InventoryPages;
};
