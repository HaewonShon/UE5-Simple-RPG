// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextActor.h"
#include "DamageTextWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADamageTextActor::ADamageTextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageTextWidgetComponent"));

	if (!WidgetComponent->GetWidgetClass())
	{
		WidgetComponent->SetWidgetClass(UDamageTextWidget::StaticClass());
	}
	SetRootComponent(WidgetComponent);
}

// Called every frame
void ADamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LocalLifetime -= DeltaTime;
	if (LocalLifetime < 0.f)
	{
		Destroy();
	}
	else
	{
		float Alpha = FMath::Clamp(LocalLifetime / Lifetime, 0.f, 1.f);
		float Opacity = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 1.f);
		DamageTextWidget->SetOpacity(Opacity);

		FVector Location = GetActorLocation();
		FRotator LookAt = CamManager->GetCameraRotation();
		LookAt.Add(0.f, 180.f, 0.f);
		LookAt.Pitch = 360.f - LookAt.Pitch;
		SetActorLocationAndRotation(Location + FVector(0.f, 0.f, FloatingSpeed * DeltaTime), LookAt);
	}
}

void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();

	DamageTextWidget = Cast<UDamageTextWidget>(WidgetComponent->GetUserWidgetObject());
	InitializeText();

	LocalLifetime = Lifetime;

	// get local camera to track
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		CamManager = PC->PlayerCameraManager.Get();
	}
}

void ADamageTextActor::SetDamageValue(float DamageValue, bool bCrit)
{
	Damage = DamageValue;
	bIsCrit = bCrit;
	UE_LOG(LogTemp, Log, TEXT("SetDamageValue in actor: %f"), Damage);
	InitializeText();
}

void ADamageTextActor::InitializeText()
{
	if (!DamageTextWidget.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("initialize failed"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Damage in actor: %f"), Damage);
	DamageTextWidget->InitializeText(Damage, bIsCrit);
}

