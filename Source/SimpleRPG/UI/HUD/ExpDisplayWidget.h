// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ProgressDisplayWidget.h"
#include "ExpDisplayWidget.generated.h"

/**
 *		
 */
UCLASS()
class SIMPLERPG_API UExpDisplayWidget : public UProgressDisplayWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void OnExpChanged(int32 NewValue, int32 MaxValue);
};
