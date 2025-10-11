// Fill out your copyright notice in the Description page of Project Settings.


#include "GEExecCalculation.h"
#include "CharacterAttributeSet.h"

struct FDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);

    FDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, AttackPower, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, CritChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, Defense, Target, false);
    }
};

static const FDamageStatics& DamageStatics()
{
    static FDamageStatics Statics;
    return Statics;
}

UGEExecCalculation::UGEExecCalculation()
{
    RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
    RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
    RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UGEExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    constexpr float CRIT_MULTIPLIER = 1.3f;

    // Check if both have ASC
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    if (!SourceASC || !TargetASC) return;

    const FGameplayEffectSpec& SkillSpec = ExecutionParams.GetOwningSpec();

    FAggregatorEvaluateParameters EvalParams;
    float AttackPower = 0.f;
    float CritChance = 0.f;
    float Defense = 0.f;

    if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvalParams, AttackPower))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read AttackPower"));
    }
    if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvalParams, CritChance))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read CritChanceDef"));
    }
    if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvalParams, Defense))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read DefenseDef"));
    }
    
    float SkillDamageMultiplier = SkillSpec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DamageMultiplier"), false, 1.f);

    float Damage = AttackPower * SkillDamageMultiplier;
    if (FMath::FRand() <= CritChance)
    {
        AttackPower *= CRIT_MULTIPLIER;
    }

    Damage = FMath::Max(Damage - Defense, 0.f);
    UE_LOG(LogTemp, Log, TEXT("Damage Dealt: %f"), AttackPower);
    OutExecutionOutput.AddOutputModifier(
        FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
}
