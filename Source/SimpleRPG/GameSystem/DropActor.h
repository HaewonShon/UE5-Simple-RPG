// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropActor.generated.h"

UCLASS(Abstract)
class SIMPLERPG_API ADropActor : public AActor
{
	GENERATED_BODY()

public:
	ADropActor();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void SetPickupDelay(float Delay);

protected:
	// ===== World Interaction =====
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void OnFloorHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	void LaunchRandomDirection();
	void StartFloating();

	// ===== Components =====
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;

	// ===== Movement / Visual =====
	void SetVFXColor(FLinearColor Color);

	UPROPERTY(EditAnywhere, Category = "Drop")
	float RotationRate = 60.f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float FloatingRate = 2.f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float FloatingRange = 20.f;

	// ===== State =====
	FVector BaseLocation;
	float ElapsedTime = 0.f;
	bool bIsFloating = false;

	static constexpr float DefaultPickupDelay = 0.5f;
	float PickupDelay = DefaultPickupDelay;
};