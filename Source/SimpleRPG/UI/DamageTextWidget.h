// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

/**
 *	A Damage Text spawned when actual damage happen.
 */
UCLASS()
class SIMPLERPG_API UDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeText(float Damage, bool bIsCrit);

	void SetOpacity(float Opacity);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DamageText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Property")
	FColor NormalHitColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Property")
	FColor CritHitColor;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UTextBlock> CritDamageText;
};
