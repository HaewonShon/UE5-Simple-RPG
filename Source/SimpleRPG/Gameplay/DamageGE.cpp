// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageGE.h"
#include "GEExecCalculation.h"

UDamageGE::UDamageGE()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayEffectExecutionDefinition ExecDef;
    ExecDef.CalculationClass = UGEExecCalculation::StaticClass();
    Executions.Add(ExecDef);
}