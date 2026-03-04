// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Shared/Common/SlotTypes.h"
#include "SlotDragDropOp.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLERPG_API USlotDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	FSlotAddress SlotAddress;
};
