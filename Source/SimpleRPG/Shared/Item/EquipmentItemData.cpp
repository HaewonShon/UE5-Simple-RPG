// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentItemData.h"
#include "Player/Components/InventoryComponent.h"
//#include "Player/Components/EquipmentComponent.h"

bool UEquipmentItemData::CanExecute(AActor* Executer, const FSlotAddress& Address) const
{
	return false;
}

void UEquipmentItemData::Execute(AActor* Executer, const FSlotAddress& Address)
{
	// request equip from inventory
	if (Address.ContainerType == ESlotType::Storage)
	{
		if (UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>())
		{
			//Inventory->RequestEquipment();
		}
	}
	// request unequip from equipment
	else
	{

	}
}

FItemDescription UEquipmentItemData::BuildDescriptionData() const
{
	FItemDescription Description;
	Description.Name = DisplayName;
	Description.Icon = Icon.Get();
	Description.Rarity = Rarity;

	FEquipmentDetail Detail;
	Detail.TypeText = UEnum::GetDisplayValueAsText(EquipmentType);
	if (Stat.AttackPower != 0) Detail.Stats.Add({ FText::FromString("AttackPower"), FText::AsNumber(Stat.AttackPower) });
	if (Stat.Defense != 0) Detail.Stats.Add({ FText::FromString("Defense"), FText::AsNumber(Stat.Defense) });
	if (Stat.CritChance != 0) Detail.Stats.Add({ FText::FromString("CritChance"), FText::AsNumber(Stat.CritChance) });
	if (Stat.MaxHealth != 0) Detail.Stats.Add({ FText::FromString("MaxHealth"), FText::AsNumber(Stat.MaxHealth) });
	if (Stat.HealthRegen != 0) Detail.Stats.Add({ FText::FromString("HealthRegen"), FText::AsNumber(Stat.HealthRegen) });

	Description.Payload.Set<FEquipmentDetail>(Detail);

	return Description;
}
