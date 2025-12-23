// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventorySlotWidget.h"
#include "InventoryDragDropOp.generated.h"

/**
 *   Drag Op for slot drag-drop
 */
UCLASS()
class SIMPLERPG_API UInventoryDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	FSlotInfo DraggingSlot;
};
