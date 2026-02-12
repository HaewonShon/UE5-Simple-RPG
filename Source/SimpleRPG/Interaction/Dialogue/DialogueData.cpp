// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueData.h"

void UDialogueData::PostInitProperties()
{
	Super::PostInitProperties();
	AssetId = GetPrimaryAssetId();
}

void UDialogueData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	AssetId = GetPrimaryAssetId();
}

FPrimaryAssetId UDialogueData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("DialogueData"), GetFName());
}
