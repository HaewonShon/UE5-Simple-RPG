// Fill out your copyright notice in the Description page of Project Settings.

#include "DropActor.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ADropActor::ADropActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComponent->BodyInstance.SleepFamily = ESleepFamily::Custom;
	MeshComponent->BodyInstance.CustomSleepThresholdMultiplier = 0.1f;
	MeshComponent->BodyInstance.bNotifyRigidBodyCollision = true;
	MeshComponent->SetSimulatePhysics(true);

	ElapsedTime = 0.f;
	bIsFloating = false;
	PickupDelay = DefaultPickupDelay;
}

void ADropActor::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &ADropActor::OnFloorHit);
	LaunchRandomDirection();
}

void ADropActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFloating)
	{
		ElapsedTime += DeltaTime;

		SetActorRotation(FRotator(0.f, ElapsedTime * RotationRate, 0.f));
		SetActorLocation(
			BaseLocation +
			FVector(
				0.f,
				0.f,
				FloatingRange / 2.f - FMath::Cos(ElapsedTime * FloatingRate) * FloatingRange
			)
		);
	}
}

void ADropActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	
}

void ADropActor::OnFloorHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (bIsFloating || !OtherComp)
	{
		return;
	}

	if (Hit.ImpactNormal.Z <= 0.75f)
	{
		return;
	}

	if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		StartFloating();
	}
}

void ADropActor::SetPickupDelay(float Delay)
{
	PickupDelay = Delay;
}

void ADropActor::LaunchRandomDirection()
{
	constexpr float LaunchZForce = 500.f;
	FVector LaunchDirection(
		FMath::RandRange(-10.f, 10.f),
		FMath::RandRange(-10.f, 10.f),
		LaunchZForce
	);

	MeshComponent->AddForce(LaunchDirection * 50.f);
}

void ADropActor::StartFloating()
{
	bIsFloating = true;
	BaseLocation = GetActorLocation();

	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetEnableGravity(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADropActor::SetVFXColor(FLinearColor Color)
{
	if (NiagaraComponent)
	{
		NiagaraComponent->SetVariableLinearColor(TEXT("VFXColor"), Color);
	}
}