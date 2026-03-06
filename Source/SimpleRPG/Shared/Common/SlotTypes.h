#pragma once

#include "CoreMinimal.h"
#include "SlotTypes.generated.h"

UENUM()
enum class ESlotType : uint8
{
    Equipment = 1 << 0,
    Storage = 1 << 1,
    Shop = 1 << 2,
    Skill = 1 << 3,
    QuickSlot = 1 << 4,
};

USTRUCT()
struct FSlotAddress
{
    GENERATED_BODY()

    ESlotType ContainerType;
    int32 SlotIndex;
};