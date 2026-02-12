// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SendGameplayTagEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_SendGameplayTagEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (AActor* Owner = MeshComp->GetOwner())
    {
        FGameplayEventData Data;
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Data);
    }
}

FString UAnimNotify_SendGameplayTagEvent::GetNotifyName_Implementation() const
{
    FString EventTagString = "";
    if (EventTag.IsValid())
    {
        EventTagString = EventTag.ToString();
    }
    
    return EventTagString.IsEmpty() ? FString("SendGameplayTagEvent") : EventTagString;
}
