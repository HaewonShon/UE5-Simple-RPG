// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableItemData.h"
#include "Player/Components/InventoryComponent.h"

bool UConsumableItemData::CanExecute(AActor* Executer, const FSlotAddress& Address) const
{
	if (Address.ContainerType == ESlotType::Storage)
	{
		UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>();
		if(Inventory && Inventory->GetCurrentMode() == EInventoryMode::Normal)
		{
			return true;
		}
	}

	return false;
}

void UConsumableItemData::Execute(AActor* Executer, const FSlotAddress& Address)
{
	if (Address.ContainerType == ESlotType::Shop)
	{
		Super::Execute(Executer, Address);
		return;
	}

	if (Address.ContainerType == ESlotType::Storage)
	{
		if (UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>())
		{
			Inventory->RequestUseItem(Address.SlotIndex);
		}
	}
}

FItemDescription UConsumableItemData::BuildDescriptionData() const
{
	FItemDescription Description;
	Description.Name = DisplayName;
	Description.Icon = Icon.Get();
	Description.Rarity = Rarity;

	FConsumableDetail Detail;
	Detail.DetailText = DescriptionText;

	Description.Payload.Set<FConsumableDetail>(Detail);

	return Description;
}
