// Fill out your copyright notice in the Description page of Project Settings.


#include "PreviewActor.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
APreviewActor::APreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SetRootComponent(SkeletalMeshComponent); 

	SkeletalMeshComponent->AddLocalOffset(FVector(0.f, 0.f, -90.f));
	SkeletalMeshComponent->AddLocalRotation(FRotator(0.f, 90.f, 0.f));

	SkeletalMeshComponent->SetMobility(EComponentMobility::Movable);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComponent->bCastDynamicShadow = false;
	SkeletalMeshComponent->CastShadow = false;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComp"));
	SceneCaptureComponent->SetupAttachment(SkeletalMeshComponent);

	SceneCaptureComponent->SetRelativeLocation(FVector(-450.f, 0.f, 0.f));
	SceneCaptureComponent->SetRelativeRotation(FRotator::ZeroRotator);
	SceneCaptureComponent->SetRelativeScale3D(FVector::OneVector);

	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;
	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	SceneCaptureComponent->ShowFlags.MotionBlur = false;
	SceneCaptureComponent->ShowFlags.TemporalAA = false;
}

void APreviewActor::BeginPlay()
{
	Capture();
}

void APreviewActor::Capture()
{
	if (SceneCaptureComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("Preview captured"));
		SceneCaptureComponent->CaptureScene();
	}
}