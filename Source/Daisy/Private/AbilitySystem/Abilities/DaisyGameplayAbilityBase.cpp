// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Character/BattleEnemy.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"


void UDaisyGameplayAbilityBase::ApplyGameplayEffectToEnemy(TArray<ABattleEnemy*> Enemy,
                                                           TSubclassOf<UGameplayEffect> InstantGameplayEffectClass)
{
	for (auto EnemyBase : Enemy)
	{
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EnemyBase);
		if(SourceASC == nullptr) return;

		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(InstantGameplayEffectClass,1,EffectContextHandle);
		FDaisyGameplayTags GameplayTags = FDaisyGameplayTags::Get();
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,GameplayTags.Damage,ScaledDamage);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		int32 a =0;
	}
}
