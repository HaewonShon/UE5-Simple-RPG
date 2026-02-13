// Fill out your copyright notice in the Description page of Project Settings.


#include "ExploreCheckActor.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Interaction/Quest/QuestManagerSubsystem.h"

// Sets default values
AExploreCheckActor::AExploreCheckActor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionObjectType(ECC_WorldStatic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetHiddenInGame(true);
	BoxComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AExploreCheckActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AExploreCheckActor::OnTriggerBeginOverlap);
}

void AExploreCheckActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	ASimpleRPGPlayerState* PlayerState = Cast<ASimpleRPGPlayerState>(Character->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}
	UE_LOG(LogTemp, Verbose, TEXT("AExploreCheckActor OnTriggerBeginOverlap called for %s"), *PlaceTag.ToString());

	if (UQuestManagerSubsystem* Subsystem = GetWorld()->GetSubsystem<UQuestManagerSubsystem>())
	{
		Subsystem->OnPlaceVisited(PlaceTag, PlayerState);
	}
}
