// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "ItemActor.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	void OnActorSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

	void SetItem(FItemInstance Item);

protected:
	void LaunchRandomDirection();

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor EquipmentColor;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor ConsumableColor;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FLinearColor MaterialColor;

	UPROPERTY(EditAnywhere, Category = "Item")
	float RotationRate;

	UPROPERTY(EditAnywhere, Category = "Item")
	float FloatingRate;

	UPROPERTY(EditAnywhere, Category = "Item")
	float FloatingRange;

	FItemInstance ItemInstance;

	FVector BaseLocation;

	float ElapsedTime;
	bool bIsFloating;
};
