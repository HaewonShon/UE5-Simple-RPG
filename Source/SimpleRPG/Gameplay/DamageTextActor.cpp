// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextActor.h"
#include "Components/WidgetComponent.h"
#include "../UI/DamageTextWidget.h"
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

	FVector Location = GetActorLocation();
	Location.Z += FloatingSpeed * DeltaTime;
	SetActorLocation(Location);

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
	}

	/*APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector CameraLocation = CameraManager->GetCameraLocation();

	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraLocation);
	LookAt.Pitch = 0.f;
	SetActorRotation(LookAt);*/
}

void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();

	DamageTextWidget = Cast<UDamageTextWidget>(WidgetComponent->GetUserWidgetObject());
	InitializeText();

	LocalLifetime = Lifetime;
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

