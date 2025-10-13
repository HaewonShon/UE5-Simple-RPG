// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTextActor.generated.h"

UCLASS()
class SIMPLERPG_API ADamageTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageTextActor();
	virtual void Tick(float DeltaTime) override;

	void SetDamageValue(float DamageValue, bool Crit);

protected:
	virtual void BeginPlay() override;
	void InitializeText();

	UPROPERTY(EditDefaultsOnly);
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	TWeakObjectPtr<class UDamageTextWidget> DamageTextWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TextBehavior")
	float FloatingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TextBehavior")
	float Lifetime;

	float Damage;
	float LocalLifetime;
	bool bIsCrit;
};
