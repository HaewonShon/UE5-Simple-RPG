// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionableAsset.h"
#include "ActionSlot.generated.h"

USTRUCT()
struct FActionSlot
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Slot")
    TObjectPtr<UActionableAsset> ContentAsset;

    UPROPERTY(EditAnywhere, Category = "Slot")
    int32 Quantity;

    UPROPERTY(EditAnywhere, Category = "Slot")
    FSlotAddress SlotAddress;

public:
    /*** helper functions ***/
    FActionSlot() : ContentAsset(nullptr), Quantity(0) {}
    bool IsEmpty() const { return ContentAsset == nullptr || Quantity <= 0; }
    void Execute(AActor* Executer)
    {
        if (!IsEmpty())
        {
            ContentAsset->Execute(Executer, SlotAddress);
        }
    }
};