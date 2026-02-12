// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CurrencyComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrencyAmountChanged, int32)


UENUM(BlueprintType)
enum class ECurrencyType : uint8
{
	Gold,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ECurrencyType, ECurrencyType::Count); // Register Enum Range using Count

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLERPG_API UCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCurrencyComponent();

	bool CanAddCurrency(ECurrencyType CurrencyType, int32 Amount);
	bool TryAddCurrency(ECurrencyType CurrencyType, int32 Amount);
	FORCEINLINE int32 GetCurrencyAmount(ECurrencyType CurrencyType) const { return CurrencyAmounts.FindRef(CurrencyType); }

	FOnCurrencyAmountChanged OnGoldAmountChanged;

protected:
	FORCEINLINE bool IsValidCurrencyType(ECurrencyType Type) {	return Type < ECurrencyType::Count;	}

	virtual void BeginPlay() override;
	bool AddCurrency(ECurrencyType CurrencyType, int32 Amount);

	UPROPERTY(EditDefaultsOnly)
	TMap<ECurrencyType, int32> CurrencyMaxAmount;

	TMap<ECurrencyType, int32> CurrencyAmounts;
};
