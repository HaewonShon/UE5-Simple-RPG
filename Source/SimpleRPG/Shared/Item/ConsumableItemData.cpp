// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableItemData.h"
#include "Player/Components/InventoryComponent.h"
//#include "Player/Components/EquipmentComponent.h"

bool UConsumableItemData::CanExecute(AActor* Executer, const FSlotAddress& Address) const
{
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
			//Inventory->RequestEquipment();
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
	if (Effect.Health != 0) Detail.Effects.Add({ FText::FromString("HP"), FText::AsNumber(Effect.Health) });
	if (Effect.Mana != 0) Detail.Effects.Add({ FText::FromString("MP"), FText::AsNumber(Effect.Mana) });

	Description.Payload.Set<FConsumableDetail>(Detail);

	return Description;
}
