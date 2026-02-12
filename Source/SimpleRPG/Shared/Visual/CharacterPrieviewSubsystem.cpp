// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPrieviewSubsystem.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/LevelStreamingDynamic.h"

void UCharacterPrieviewSubsystem::RequestPreview()
{
	UWorld* World = GetWorld();

	UE_LOG(LogTemp, Error, TEXT(
		"[PreviewSubsystem] World=%s | Type=%d | Game=%d | BegunPlay=%d | NetMode=%d"),
		*World->GetName(),
		(int32)World->WorldType,
		World->IsGameWorld(),
		World->HasBegunPlay(),
		(int32)World->GetNetMode()
	);

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
		UE_LOG(LogTemp, Error, TEXT("Failed to load preview level"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to load preview level1"));
	StreamingLevel->OnLevelLoaded.AddDynamic(
		this, &UCharacterPrieviewSubsystem::CapturePreview);

	UE_LOG(LogTemp, Error, TEXT("Failed to load preview level2"));
	if (StreamingLevel->IsLevelLoaded())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load preview level3"));
		CapturePreview();
	}

	UE_LOG(LogTemp, Warning, TEXT("ShouldBeLoaded: %d"), StreamingLevel->ShouldBeLoaded());
	UE_LOG(LogTemp, Warning, TEXT("ShouldBeVisible: %d"), StreamingLevel->ShouldBeVisible());
	UE_LOG(LogTemp, Warning, TEXT("IsLevelLoaded: %d"), StreamingLevel->IsLevelLoaded());
	UE_LOG(LogTemp, Warning, TEXT("IsLevelVisible: %d"), StreamingLevel->IsLevelVisible());

}

void UCharacterPrieviewSubsystem::CapturePreview()
{
	UE_LOG(LogTemp, Error, TEXT("Capturing character preview."));
	for (AActor* Actor : StreamingLevel->GetWorld()->ActiveGroupActors)
	{
		if (USceneCaptureComponent2D* Component = Actor->GetComponentByClass<USceneCaptureComponent2D>())
		{
			Component->CaptureScene();
			UE_LOG(LogTemp, Error, TEXT("Successfully Captured character preview."));
			return;
		}
	}
}