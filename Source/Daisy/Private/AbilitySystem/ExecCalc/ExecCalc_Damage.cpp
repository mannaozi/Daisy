// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "DaisyGameplayTags.h"
#include "Game/BattleManager.h"
#include "AbilitySystem/DaisyAttributeSet.h"
#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"

struct DaisyDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Critical);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	

	DaisyDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDaisyAttributeSet,Defense,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDaisyAttributeSet,Attack,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDaisyAttributeSet,Critical,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDaisyAttributeSet,CriticalDamage,Source,false);
	}
};

static const DaisyDamageStatics& DamageStatics()
{
	static DaisyDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	//找到Ability的伤害类型，更改damage颜色
	FGameplayTag Tag = Cast<UDaisyGameplayAbilityBase>(EffectContextHandle.GetAbility())->AttackTag;
	FColor DamageColor = UDaisyBlueprintFunctionLibrary::FindBattleManager()->GetColorByTag(Tag);
	UDaisyBlueprintFunctionLibrary::SetDamageColor(EffectContextHandle,DamageColor);
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Damage = Spec.GetSetByCallerMagnitude(FDaisyGameplayTags::Get().Damage);
	//攻击力
	float Attack = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef,EvaluateParameters,Attack);
	Damage *= Attack;
	
	//暴击率
	float Critical = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDef,EvaluateParameters,Critical);
	Critical = FMath::Max<float>(0, Critical);
	const bool bCriticalHit = FMath::RandRange(1,100) < Critical;

	UDaisyBlueprintFunctionLibrary::SetIsCriticalHit(EffectContextHandle,bCriticalHit);
	
	//暴击额外伤害
	float CriticalDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalDamageDef,EvaluateParameters,CriticalDamage);
	CriticalDamage = FMath::Max<float>(0, CriticalDamage);
	if (bCriticalHit) Damage = Damage * CriticalDamage * 0.01f;
	
	//防御
	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef,EvaluateParameters,Defense);
	Defense = FMath::Max<float>(0.f,Defense);
	Damage = Damage / Defense;
	
	
	const FGameplayModifierEvaluatedData EvaluatedData(UDaisyAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
