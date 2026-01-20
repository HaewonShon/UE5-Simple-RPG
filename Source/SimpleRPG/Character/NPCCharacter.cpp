// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacter.h"
#include "../Quest/QuestGiverComponent.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "../Dialogue/DialogueComponent.h"
#include "InteractionRotationComponent.h"

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

void ANPCCharacter::Interact(ACharacter* Character, ASimpleRPGPlayerController* PS)
{
	/*if (UQuestGiverComponent* Comp = GetComponentByClass<UQuestGiverComponent>())
	{
		UE_LOG(LogNPC, Log, TEXT("Quest requested"));
		Comp->Oninteraction(GetPrimaryAssetId(), PS);
	}*/

	if (UDialogueComponent* Comp = GetComponentByClass<UDialogueComponent>())
	{
		UE_LOG(LogNPC, Log, TEXT("Quest requested"));
		Comp->Interact(PS);
	}
	if (UInteractionRotationComponent* Comp = GetComponentByClass<UInteractionRotationComponent>())
	{
		Comp->StartRoationToTarget(Character);
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
	UE_LOG(LogNPC, Log, TEXT("Begin overlap"));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->SetInteractableNPC(this);
	}
}

void ANPCCharacter::OnInteractRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogNPC, Log, TEXT("End overlap"));
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		Player->ClearInteractableNPC(this);
	}
}