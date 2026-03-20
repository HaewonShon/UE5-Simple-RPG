// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "Player/PlayerCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemInstance = FItemInstance();
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (ItemInstance.IsValid())
	{
		SetItem(ItemInstance);
	}
}

void AItemActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (ElapsedTime < PickupDelay)
	{
		return;
	}

	if (APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor))
	{
		bool bResult = Character->AddItem(ItemInstance);
		if (bResult)
		{
			this->Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to add items"));
		}
	}
}

void AItemActor::SetItem(FItemInstance Item)
{
	ItemInstance = Item;

	// destroy self if invalid
	if (!ItemInstance.DataAsset.IsValid())
	{
		Destroy();
	}

	FLinearColor VFXColor = FLinearColor::White;
	switch (Item.DataAsset->Category)
	{
	case EItemCategory::Equipment:
		VFXColor = EquipmentColor;
		break;
	case EItemCategory::Consumable:
		VFXColor = ConsumableColor;
		break;
	case EItemCategory::Material:
		VFXColor = MaterialColor;
		break;
	}
	SetVFXColor(VFXColor);
}