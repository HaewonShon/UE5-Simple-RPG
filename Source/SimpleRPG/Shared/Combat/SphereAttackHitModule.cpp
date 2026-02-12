// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereAttackHitModule.h"
#include "DrawDebugHelpers.h"
#include "Enemy/Enemy.h"

TArray<AActor*> USphereAttackHitModule::GetHitTargets(AActor* Instigator)
{
    check(TargetClass)

    UWorld* World = GetWorld();
    ensure(World);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ChannelToHit);

    FCollisionQueryParams Params;
    Params.bTraceComplex = false;
    Params.bReturnPhysicalMaterial = false;

    FVector ActorPosition = Instigator->GetActorLocation();
    FVector Forward = Instigator->GetActorForwardVector();

    FVector SphereCenter = ActorPosition + Forward * Range;

    if (bShouldDrawDebugInfo)
    {
        DebugDraw(SphereCenter);
    }

    TArray<FHitResult> HitResults;
    // Sphere Sweep
    bool bHit = World->SweepMultiByObjectType(
        HitResults,
        SphereCenter,
        SphereCenter,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    TArray<AActor*> HitEnemies;

    for (FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->GetClass()->IsChildOf(TargetClass))
        {
            HitEnemies.Add(HitActor);
        }
    }

	return HitEnemies;
}

void USphereAttackHitModule::DebugDraw(const FVector& Location)
{
    DrawDebugSphere(GetWorld(), Location, Radius, 16, DebugDrawColor, false, 2.0f);
}
