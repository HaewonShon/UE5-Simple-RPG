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

	// capture component for portrait
	PortraitCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PortraitSceneCaptureComp"));
	PortraitCaptureComponent->SetupAttachment(SkeletalMeshComponent);

	PortraitCaptureComponent->SetRelativeLocation(FVector(-450.f, 0.f, 0.f));
	PortraitCaptureComponent->SetRelativeRotation(FRotator::ZeroRotator);
	PortraitCaptureComponent->SetRelativeScale3D(FVector::OneVector);

	PortraitCaptureComponent->bCaptureEveryFrame = false;
	PortraitCaptureComponent->bCaptureOnMovement = false;
	PortraitCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	PortraitCaptureComponent->ShowFlags.MotionBlur = false;
	PortraitCaptureComponent->ShowFlags.TemporalAA = false;

	// capture component for full-body image
	FullBodyCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FullBodySceneCaptureComp"));
	FullBodyCaptureComponent->SetupAttachment(SkeletalMeshComponent);

	FullBodyCaptureComponent->SetRelativeLocation(FVector(-450.f, 0.f, 0.f));
	FullBodyCaptureComponent->SetRelativeRotation(FRotator::ZeroRotator);
	FullBodyCaptureComponent->SetRelativeScale3D(FVector::OneVector);

	FullBodyCaptureComponent->bCaptureEveryFrame = false;
	FullBodyCaptureComponent->bCaptureOnMovement = false;
	FullBodyCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	FullBodyCaptureComponent->ShowFlags.MotionBlur = false;
	FullBodyCaptureComponent->ShowFlags.TemporalAA = false;
}

void APreviewActor::BeginPlay()
{
	Capture();
}

void APreviewActor::Capture()
{
	if (PortraitCaptureComponent)
	{
		PortraitCaptureComponent->CaptureScene();
	}

	if (FullBodyCaptureComponent)
	{
		FullBodyCaptureComponent->CaptureScene();
	}
}