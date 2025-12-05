// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"	

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponoent"));

	//MeshComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetupAttachment(RootComponent);

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block); 
	MeshComponent->BodyInstance.SleepFamily = ESleepFamily::Custom;
	MeshComponent->BodyInstance.CustomSleepThresholdMultiplier = 10.f;
	MeshComponent->SetSimulatePhysics(true);

	ElapsedTime = 0.0f;
	bIsFloating = false;

	ItemInstance = FItemInstance();
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentSleep.AddDynamic(this, &AItemActor::OnActorSleep);
	LaunchRandomDirection();
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT("Sleeping? : %i"), !MeshComponent->IsAnyRigidBodyAwake());

	if (bIsFloating)
	{
		ElapsedTime += DeltaTime;

		SetActorRotation(FRotator(0.f, 0.f, ElapsedTime * RotationRate));
		SetActorLocation(BaseLocation + FVector(0.f, 0.f, -FMath::Cos(ElapsedTime * FloatingRate) * FloatingRange));
	}
}

void AItemActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("Item Overlapped"));
}

UFUNCTION()
void AItemActor::OnActorSleep(UPrimitiveComponent* SleepingComponent, FName BoneName)
{
	UE_LOG(LogTemp, Log, TEXT("Item Sleep"));

	MeshComponent->SetSimulatePhysics(false);
	bIsFloating = true;
	BaseLocation = GetActorLocation();
}

void AItemActor::SetItem(FItemInstance Item)
{
	ItemInstance = Item;

	if (NiagaraComponent)
	{
		FLinearColor VFXColor = FLinearColor::White;

		switch (Item.ItemData->Category)
		{
		case EItemCategory::Weapon:
		case EItemCategory::Armor:
			VFXColor = EquipmentColor;
			break;
		case EItemCategory::Consumable:
			VFXColor = ConsumableColor;
			break;
		case EItemCategory::Material:
			VFXColor = MaterialColor;
			break;
		}
		NiagaraComponent->SetVariableLinearColor(TEXT("VFXColor"), VFXColor);
	}
}

void AItemActor::LaunchRandomDirection()
{
	constexpr float LaunchZForce = 100.f;
	FVector LaunchDirection(FMath::RandRange(-10.f, 10.f), FMath::RandRange(-10.f, 10.f), LaunchZForce);
	MeshComponent->BodyInstance.AddForce(LaunchDirection);
}