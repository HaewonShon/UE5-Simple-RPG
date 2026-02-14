// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacter.h"
#include "Components/SphereComponent.h"
#include "Interaction/Quest/QuestGiverComponent.h"
#include "Interaction/Dialogue/DialogueComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/SimpleRPGPlayerController.h"
#include "Shared/Utils/InteractionRotationComponent.h"

DEFINE_LOG_CATEGORY(LogNPC);

// Sets default values
ANPCCharacter::ANPCCharacter()
{
	InteractRangeSphere = CreateDefaultSubobject <USphereComponent>(TEXT("InteractRangeSphere")); InteractRangeSphere->SetupAttachment(RootComponent);
	InteractRangeSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractRangeSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractRangeSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractRangeSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	InteractRangeSphere->SetSphereRadius(150.f);
}

void ANPCCharacter::Interact(ACharacter* Character, ASimpleRPGPlayerController* PC)
{
	if (UDialogueComponent* DialogoueComponent = GetComponentByClass<UDialogueComponent>())
	{
		UE_LOG(LogNPC, Log, TEXT("Dialogue interaction with %s requested"), *GetName());
		PC->OnDialogueRequested.Broadcast(this);
	}

	if (UInteractionRotationComponent* RotationComponent = GetComponentByClass<UInteractionRotationComponent>())
	{
		RotationComponent->StartRoationToTarget(Character);
	}
}

FPrimaryAssetId ANPCCharacter::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("NPC"), GetFName());
}

// Called when the game starts or when spawned
void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacter::OnInteractRangeBeginOverlap);
	InteractRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacter::OnInteractRangeEndOverlap);
}

void ANPCCharacter::OnInteractRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->SetInteractableNPC(this);
	}
}

void ANPCCharacter::OnInteractRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->ClearInteractableNPC(this);
	}
}