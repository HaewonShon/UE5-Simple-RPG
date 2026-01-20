// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionRotationComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UInteractionRotationComponent::UInteractionRotationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInteractionRotationComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultRotation = GetOwner()->GetActorRotation();
}

void UInteractionRotationComponent::StartRoationToTarget(AActor* Target)
{
	AActor* Owner = GetOwner();
	FVector ToTarget = Target->GetActorLocation() - Owner->GetActorLocation();
	ToTarget.Z = 0.f;
	TargetRotation = ToTarget.Rotation();

	// ignore pitch & roll
	TargetRotation.Pitch = 0.f;
	TargetRotation.Roll = 0.f;

	GetWorld()->GetTimerManager().SetTimer(
		RotationTimerHandle,
		this,
		&UInteractionRotationComponent::UpdateRotationToTarget,
		0.016f, // 60fps
		true
	);
}

void UInteractionRotationComponent::UpdateRotationToTarget()
{
	AActor* Owner = GetOwner();
	FRotator CurrentRotation = Owner->GetActorRotation();

	float YawDiff = FMath::Abs(
		FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw)
	);

	if (YawDiff <= AcceptableYawDiff)
	{
		Owner->SetActorRotation(TargetRotation);
		GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
		return;
	}

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation,
		GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);

	NewRotation.Pitch = CurrentRotation.Pitch;
	NewRotation.Roll = CurrentRotation.Roll;

	Owner->SetActorRotation(NewRotation);
}

