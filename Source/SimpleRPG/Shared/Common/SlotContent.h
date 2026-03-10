// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionableAsset.h"
#include "SlotContent.generated.h"

USTRUCT()
struct FSlotContent
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Slot")
    TWeakObjectPtr<UActionableAsset> ContentAsset;

    UPROPERTY(EditAnywhere, Category = "Slot")
    int32 Quantity;

    UPROPERTY(EditAnywhere, Category = "Slot")
    FSlotAddress SlotAddress;

public:
    /*** helper functions ***/
    FSlotContent() : ContentAsset(nullptr), Quantity(0) {}
    FSlotContent(UActionableAsset* Asset) : ContentAsset(Asset), Quantity(1) {}
    inline bool IsEmpty() const { return ContentAsset == nullptr || Quantity <= 0; }
    void Execute(AActor* Executer)
    {
        if (!IsEmpty())
        {
            ContentAsset->Execute(Executer, SlotAddress);
        }
    }
};