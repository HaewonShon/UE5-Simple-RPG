// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"
#include "NiagaraComponent.h"
#include "../Character/PlayerCharacter.h"
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
	MeshComponent->BodyInstance.CustomSleepThresholdMultiplier = 0.1f;
	MeshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
	MeshComponent->SetSimulatePhysics(true);

	ElapsedTime = 0.0f;
	bIsFloating = false;
	PickupDelay = DefaultPickupDelay;

	ItemInstance = FItemInstance();
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &AItemActor::OnFloorHit);
	LaunchRandomDirection();
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFloating)
	{
		ElapsedTime += DeltaTime;

		SetActorRotation(FRotator(0.f, ElapsedTime * RotationRate, 0.f));
		bool bRes = SetActorLocation(BaseLocation + FVector(0.f, 0.f, FloatingRange / 2.f - FMath::Cos(ElapsedTime * FloatingRate) * FloatingRange));
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

void AItemActor::OnFloorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsFloating || !OtherComp)
	{
		return;
	}

	if (Hit.ImpactNormal.Z <= 0.75f) // not floor hit normal 
	{
		return;
	}

	ECollisionChannel Channel = OtherComp->GetCollisionObjectType();
	if (Channel == ECC_WorldStatic)
	{
		if (!bIsFloating)
		{
			StartFloating();
		}
	}
}

void AItemActor::SetItem(FItemInstance Item)
{
	ItemInstance = Item;

	if (NiagaraComponent)
	{	
		FLinearColor VFXColor = FLinearColor::White;

		switch (Item.ItemData->Category)
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
		NiagaraComponent->SetVariableLinearColor(TEXT("VFXColor"), VFXColor);
	}
}

void AItemActor::SetPickupDelay(float Delay)
{
	PickupDelay = Delay;
}

void AItemActor::LaunchRandomDirection()
{
	UE_LOG(LogTemp, Verbose, TEXT("Item Launched!"));
	constexpr float LaunchZForce = 500.f;
	FVector LaunchDirection(FMath::RandRange(-10.f, 10.f), FMath::RandRange(-10.f, 10.f), LaunchZForce);
	MeshComponent->BodyInstance.AddForce(LaunchDirection * 50.f);
}

void AItemActor::StartFloating()
{
	UE_LOG(LogTemp, Verbose, TEXT("StartFloating"));

	bIsFloating = true;
	BaseLocation = GetActorLocation();

	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//MeshComponent->SyncComponentToRBPhysics();
}