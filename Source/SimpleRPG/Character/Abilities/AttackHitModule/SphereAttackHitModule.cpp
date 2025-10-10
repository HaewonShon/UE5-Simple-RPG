// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereAttackHitModule.h"

TArray<AActor*> USphereAttackHitModule::GetHitTargets(AActor* Instigator)
{
	if (bShouldDrawDebugInfo)
	{
		DebugDraw();
	}

    UWorld* World = GetWorld();

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionQueryParams Params;
    Params.bTraceComplex = false;
    Params.bReturnPhysicalMaterial = false;

    // Hit 결과 배열
    TArray<FHitResult> HitResults;

    // Sphere Sweep
    bool bHit = World->SweepMultiByObjectType(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

	return TArray<AActor*>();
}

void USphereAttackHitModule::DebugDraw()
{
}
