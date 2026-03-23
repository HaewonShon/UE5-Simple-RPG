// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Enhancement/EnhancementComponent.h"
#include "Player/SimpleRPGPlayerState.h"
#include "Player/SimpleRPGPlayerController.h"
#include "Shared/Item/ItemManagementSubsystem.h"

DEFINE_LOG_CATEGORY(LogEnhancement)

// Sets default values for this component's properties
UEnhancementComponent::UEnhancementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<FActionInfo> UEnhancementComponent::CreateAvailableActions(ASimpleRPGPlayerState* PS)
{
	TArray<FActionInfo> Actions;

	FActionInfo Action;
	Action.ContextOwner = this;
	Action.DisplayName = FText::FromString("Enhancement");
	Action.Icon = nullptr;
	Action.Type = EActionType::Enhancement;

	ASimpleRPGPlayerController* PC = Cast<ASimpleRPGPlayerController>(PS->GetPlayerController());
	Action.OnActionExecuted.BindUObject(PC, &ASimpleRPGPlayerController::OpenEnhancement, this);

	Actions.Add(Action);
	return Actions;
}

FActionInfo UEnhancementComponent::CreateContextAction(FGameplayTag ActionTag, ASimpleRPGPlayerState* PS)
{
	return FActionInfo();
}
