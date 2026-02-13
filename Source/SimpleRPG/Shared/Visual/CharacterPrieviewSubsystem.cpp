// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPrieviewSubsystem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/LevelStreamingDynamic.h"

void UCharacterPrieviewSubsystem::RequestPreview()
{
	UWorld* World = GetWorld();
	
	static const FSoftObjectPath PreviewLevelPath(
		TEXT("/Content/Maps/PreviewRenderLevel.PreviewRenderLevel")
	);
	TSoftObjectPtr<UWorld> PreviewLevelAsset(PreviewLevelPath);
	const FString LevelPath = TEXT("/Game/Maps/PreviewRenderLevel");

	bool bLoadLevelResult = false;
	StreamingLevel =
		ULevelStreamingDynamic::LoadLevelInstance(
			GetWorld(), 
			LevelPath,
			FVector(100000.f, 100000.f, -100000.f),
			FRotator::ZeroRotator,
			bLoadLevelResult
		);

	if (!bLoadLevelResult || !StreamingLevel)
	{
		return;
	}

	StreamingLevel->OnLevelLoaded.AddDynamic(
		this, &UCharacterPrieviewSubsystem::CapturePreview);

	if (StreamingLevel->IsLevelLoaded())
	{
		CapturePreview();
	}
}

void UCharacterPrieviewSubsystem::CapturePreview()
{
	UE_LOG(LogTemp, Verbose, TEXT("Capturing character preview."));
	for (AActor* Actor : StreamingLevel->GetWorld()->ActiveGroupActors)
	{
		if (USceneCaptureComponent2D* Component = Actor->GetComponentByClass<USceneCaptureComponent2D>())
		{
			Component->CaptureScene();
			UE_LOG(LogTemp, Log, TEXT("Successfully Captured character preview."));
			return;
		}
	}
}