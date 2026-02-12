// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeAttackHitModule.h"

TArray<AActor*> UConeAttackHitModule::GetHitTargets(AActor* Instigator)
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

    if (bShouldDrawDebugInfo)
    {
        DebugDraw(ActorPosition, Forward);
    }

    TArray<FHitResult> HitResults;
    // Sphere Sweep
    bool bHit = World->SweepMultiByObjectType(
        HitResults,
        ActorPosition,
        ActorPosition,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(Range),
        Params
    );

    TArray<AActor*> HitEnemies;

    for (FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->GetClass()->IsChildOf(TargetClass) && IsInside(HitActor, ActorPosition, Forward))
        {
            HitEnemies.Add(HitActor);
        }
    }

    return HitEnemies;
}

void UConeAttackHitModule::DebugDraw(const FVector& Location, const FVector& Forward)
{
    constexpr float DrawHeight = 200.0f;
    DrawDebugCone(GetWorld(), Location, Forward, Range, Angle, DrawHeight, 12, DebugDrawColor, false, 2.0f);
}

bool UConeAttackHitModule::IsInside(AActor* Target, const FVector& Location, const FVector& Forward) const
{
    FVector ToTarget = (Target->GetTargetLocation() - Location).GetSafeNormal();
    float CosTheta = FVector::DotProduct(ToTarget, Forward);
    float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(Angle * 0.5f));

    return CosTheta >= CosHalfAngle;
}
