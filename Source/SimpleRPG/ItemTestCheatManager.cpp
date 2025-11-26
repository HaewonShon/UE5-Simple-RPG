// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemTestCheatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SimpleRPGPlayerState.h"
#include "Character/Inventory/InventoryComponent.h"

DEFINE_LOG_CATEGORY(LogCheat)

void UItemTestCheatManager::GiveItem(int32 Index)
{
	if (Index >= ItemList.Num())
	{
		return;
	}

	ensure(GetWorld());

	TWeakObjectPtr <UInventoryComponent> InventoryComponent = GetPlayerInventoryComponent();
	if (InventoryComponent.IsValid())
	{
		FItemInstance NewItem;
		NewItem.SetItem(ItemList[Index].GetDefaultObject());
		NewItem.StackCount = 1;

		bool Result = InventoryComponent->AddItem(NewItem);

		UE_LOG(LogCheat, Log, TEXT("Item %s given to the player, result: %i"), *ItemList[Index]->GetName(), Result);
	}
}

TWeakObjectPtr<UInventoryComponent> UItemTestCheatManager::GetPlayerInventoryComponent()
{
	ASimpleRPGPlayerState* PlayerState = Cast<ASimpleRPGPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));

	if (!PlayerState)
	{
		return nullptr;
	}

	TWeakObjectPtr <UInventoryComponent> InventoryComponent = PlayerState->GetInventoryComponent();
	if (!InventoryComponent.IsValid())
	{
		return nullptr;
	}

	return InventoryComponent;
}
