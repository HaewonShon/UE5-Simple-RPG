// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32) // new level
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnExpChanged, int32, int32) // new level

USTRUCT(BlueprintType)
struct FLevelTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 Level;

	UPROPERTY(EditAnywhere)
	int32 RequiredExp;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API ULevelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelComponent();

	void GrantExp(int32 Amount);

	FOnLevelChanged OnLevelChanged;
	FOnExpChanged OnExpChanged;

protected:
	bool CanLevelUp() const;
	int32 GetRequiredExp(int32 Level) const;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UDataTable> LevelTable;

	int32 CurrentLevel;
	int32 CurrentExp;	
};