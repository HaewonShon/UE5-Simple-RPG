// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentItemData.h"
#include "Player/Components/InventoryComponent.h"
#include "Player/Components/EquipmentComponent.h"

bool UEquipmentItemData::CanExecute(AActor* Executer, const FSlotAddress& Address) const
{
	return true;
}

void UEquipmentItemData::Execute(AActor* Executer, const FSlotAddress& Address)
{
	if (Address.ContainerType == ESlotType::Shop)
	{
		Super::Execute(Executer, Address);
		return;
	}

	// request equip from inventory
	if (Address.ContainerType == ESlotType::Storage)
	{
		if (UInventoryComponent* Inventory = Executer->GetComponentByClass<UInventoryComponent>())
		{
			Inventory->RequestEquipItem(Address.SlotIndex);
		}
	}
	// request unequip from equipment
	else if(Address.ContainerType == ESlotType::Equipment)
	{
		if (UEquipmentComponent* Equipment = Executer->GetComponentByClass<UEquipmentComponent>())
		{
			Equipment->RequestRemoveEquipment(Address.SlotIndex);
		}
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
	if (BaseStat.AttackPower != 0) Detail.Stats.Add({ EStat::AttackPower, FText::AsNumber(BaseStat.AttackPower) });
	if (BaseStat.Defense != 0) Detail.Stats.Add({ EStat::Defense, FText::AsNumber(BaseStat.Defense) });
	if (BaseStat.CritChance != 0) Detail.Stats.Add({ EStat::CritChance, FText::AsNumber(BaseStat.CritChance) });
	if (BaseStat.MaxHealth != 0) Detail.Stats.Add({ EStat::MaxHealth, FText::AsNumber(BaseStat.MaxHealth) });
	if (BaseStat.HealthRegen != 0) Detail.Stats.Add({ EStat::HealthRegen, FText::AsNumber(BaseStat.HealthRegen) });

	Description.Payload.Set<FEquipmentDetail>(Detail);

	return Description;
}
