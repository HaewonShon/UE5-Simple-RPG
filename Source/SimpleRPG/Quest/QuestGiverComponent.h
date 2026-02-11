// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../GameSystem/Interactions/ActionProvider.h"
#include "GameplayTagContainer.h"
#include "QuestGiverComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLERPG_API UQuestGiverComponent : public UActorComponent, public IActionProvider
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestGiverComponent();

	void Oninteraction(FPrimaryAssetId NPCId, class ASimpleRPGPlayerState* PS);

	virtual TArray<FActionInfo> GetAvailableActions(class ASimpleRPGPlayerState* PS) const;
	virtual TArray<FActionInfo> GetContextAction(FGameplayTag ActionTag, class ASimpleRPGPlayerState* PS) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void RequestAvailableQuestList();

	UPROPERTY(EditAnywhere, Category = "Quest")
	TArray<FPrimaryAssetId> QuestList;

	TArray<FPrimaryAssetId> AvailableQuestList;
};
