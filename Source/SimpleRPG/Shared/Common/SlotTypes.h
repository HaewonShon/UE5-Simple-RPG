#pragma once

#include "CoreMinimal.h"
#include "SlotTypes.generated.h"

UENUM()
enum class ESlotType : uint8
{
    Weapon = 1 << 0,
    Helmet = 1 << 1,
    Chest = 1 << 2,
    Pants = 1 << 3,
    Boots = 1 << 4,
    Storage = 1 << 5,
    Shop = 1 << 6,
    Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(ESlotType, ESlotType::Count);

USTRUCT()
struct FSlotAddress
{
    GENERATED_BODY()

    ESlotType ContainerType;
    int32 SlotIndex;
};