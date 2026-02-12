// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestData.h"

DEFINE_LOG_CATEGORY(LogQuest);

void UQuestData::PostInitProperties()
{
	Super::PostInitProperties();
	AssetId = GetPrimaryAssetId();
}

void UQuestData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	AssetId = GetPrimaryAssetId();
}

FPrimaryAssetId UQuestData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType("QuestData"), GetFName());
}
