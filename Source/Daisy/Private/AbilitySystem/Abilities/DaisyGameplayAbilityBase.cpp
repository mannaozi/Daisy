// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Character/BattleEnemy.h"
#include "AbilitySystemBlueprintLibrary.h"


void UDaisyGameplayAbilityBase::ApplyGameplayEffectToEnemy(TArray<ABattleEnemy*> Enemy,
                                                           TSubclassOf<UGameplayEffect> InstantGameplayEffectClass)
{
	for (auto EnemyBase : Enemy)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EnemyBase);
		if(TargetASC == nullptr) return;

		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(InstantGameplayEffectClass,1,EffectContextHandle);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
