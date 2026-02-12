// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_Damage.h"
#include "GEExecCalculation.h"

UGE_Damage::UGE_Damage()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;

    FGameplayEffectExecutionDefinition ExecDef;
    ExecDef.CalculationClass = UGEExecCalculation::StaticClass();
    Executions.Add(ExecDef);
}