// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCameraActor.h"
#include "Kismet/KismetMathLibrary.h"

void ADialogueCameraActor::SetupCameraTransform(const FVector& PlayerPosition, const FVector& NPCPosition)
{
    FVector PlayerToNPC = NPCPosition - PlayerPosition;
    PlayerToNPC.Z = 0.f;
    PlayerToNPC.Normalize();

    FVector Forward = PlayerToNPC;
    FVector Right = FVector::CrossProduct(FVector::UpVector, Forward);

    // cam position in player's back-left
    FVector CameraPosition = PlayerPosition - (Forward * CameraDistance) + Right * RightOffset + FVector(0.f, 0.f, CameraHeight);

    // cam aims point between npc-player
    FVector LookAtPos = (PlayerPosition + NPCPosition) * 0.5f;

    FRotator CameraRotation = UKismetMathLibrary::FindLookAtRotation(CameraPosition, LookAtPos);

    SetActorLocationAndRotation(CameraPosition, CameraRotation);
}

void ADialogueCameraActor::ActivateCamera(APlayerController* PlayerController, float BlendTime)
{
    PlayerController->SetViewTargetWithBlend(this, BlendTime);
    
    // prevent input from player
    PlayerController->SetIgnoreMoveInput(true);
    PlayerController->SetIgnoreLookInput(true);
}

void ADialogueCameraActor::DeactivateCamera(AActor* PlayerCharacter, APlayerController* PlayerController, float BlendTime)
{
    PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime);

    PlayerController->SetIgnoreMoveInput(false);
    PlayerController->SetIgnoreLookInput(false);
}