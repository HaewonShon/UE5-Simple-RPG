// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "StatDisplayWidget.generated.h"

/**
 *	widget for displaying single stat info
 */
UCLASS(BlueprintType)
class SIMPLERPG_API UStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextBlock;
};

/**
 *	Stat display widget in Inventory using ASC
 */
UCLASS(BlueprintType)
class SIMPLERPG_API UStatDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;


};
