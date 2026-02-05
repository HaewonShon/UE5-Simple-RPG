// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrencyComponent.h"

// Sets default values for this component's properties
UCurrencyComponent::UCurrencyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Load if needed
	for (ECurrencyType Currency : TEnumRange<ECurrencyType>())
	{
		CurrencyMaxAmount.Add({ Currency, -1 });
	}
}

bool UCurrencyComponent::CanAddCurrency(ECurrencyType CurrencyType, int32 Amount)
{	
	int32 CurrentAmount = GetCurrencyAmount(CurrencyType);
	int32 MaxAmount = CurrencyMaxAmount[CurrencyType];
	MaxAmount = (MaxAmount == -1) ? INT32_MAX : MaxAmount;

	if (CurrentAmount + Amount <= MaxAmount)
	{
		return true;
	}
	return false;
}

bool UCurrencyComponent::TryAddCurrency(ECurrencyType CurrencyType, int32 Amount)
{
	if (!IsValidCurrencyType(CurrencyType))
	{
		return false;
	}

	if(CanAddCurrency(CurrencyType, Amount))
	{
		return AddCurrency(CurrencyType, Amount);
	}
	return false;
}

void UCurrencyComponent::BeginPlay()
{
	Super::BeginPlay();

	// load if needed
	for (ECurrencyType Currency : TEnumRange<ECurrencyType>())
	{
		CurrencyAmounts.Add({ Currency, 0 });
	}
}

bool UCurrencyComponent::AddCurrency(ECurrencyType CurrencyType, int32 Amount)
{	
	CurrencyAmounts[CurrencyType] += Amount;

	switch (CurrencyType)
	{
	case ECurrencyType::Gold:
		OnGoldAmountChanged.Broadcast(CurrencyAmounts[CurrencyType]);
		break;
	}

	return true;
}