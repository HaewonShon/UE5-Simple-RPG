// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldDropActor.h"
#include "../Character/PlayerCharacter.h"

// Sets default values
AGoldDropActor::AGoldDropActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGoldDropActor::SetGoldAmount(int32 Amount)
{
	GoldAmount = Amount;
	SetVFXColor(VFXColor);

	// destroy self if invalid
	if (GoldAmount <= 0)
	{
		Destroy();
	}
}

void AGoldDropActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGoldDropActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("goldcoin overlapped"));
	if (ElapsedTime < PickupDelay)
	{
		return;
	}

	if (APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor))
	{
		bool bResult = Character->AddGold(GoldAmount);
		if (bResult)
		{
			this->Destroy();
		}
	}
}
