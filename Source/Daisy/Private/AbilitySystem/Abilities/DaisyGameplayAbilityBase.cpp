// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Character/BattleEnemy.h"
#include "Character/BattlePlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"


void UDaisyGameplayAbilityBase::ApplyGameplayEffectToEnemy(TArray<ABattleEnemy*> Enemy)
{
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	for (auto EnemyBase : Enemy)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EnemyBase);
		if(SourceASC == nullptr) return;
		if(TargetASC == nullptr) return;
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(InitGameplayEffectClass,1,EffectContextHandle);
		FDaisyGameplayTags GameplayTags = FDaisyGameplayTags::Get();
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,GameplayTags.Damage,ScaledDamage);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		
		bool IsWeakness = TargetASC->HasMatchingGameplayTag(AttackTag);
		if (IsWeakness)
		{
			FGameplayEffectContextHandle EffectContextHandle_Weak = SourceASC->MakeEffectContext();
			EffectContextHandle_Weak.AddSourceObject(GetAvatarActorFromActorInfo());
			const FGameplayEffectSpecHandle EffectSpecHandle_Weak = SourceASC->MakeOutgoingSpec(WeakNessGameplayEffectClass,1,EffectContextHandle_Weak);
			const FActiveGameplayEffectHandle ActiveEffectHandle_Weak = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle_Weak.Data.Get());
		}
	}
}

void UDaisyGameplayAbilityBase::ApplyGameplayEffectToPlayer(TArray<ABattlePlayer*> Player)
{
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	for (auto PlayerBase : Player)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerBase);
		if(SourceASC == nullptr) return;
		if(TargetASC == nullptr) return;
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(InitGameplayEffectClass,1,EffectContextHandle);
		FDaisyGameplayTags GameplayTags = FDaisyGameplayTags::Get();
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,GameplayTags.Damage,ScaledDamage);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		
	}
}

void UDaisyGameplayAbilityBase::AddEnergy()
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	if(TargetASC == nullptr) return;

	check(EnergyGameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(EnergyGameplayEffectClass,1,EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

}

FActiveGameplayEffectHandle UDaisyGameplayAbilityBase::AddBuffToPlayer(ABattlePlayer* Player,TSubclassOf<UGameplayEffect> BuffGameplayEffectClass)
{
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = Player->GetAbilitySystemComponent();
	check(EnergyGameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(BuffGameplayEffectClass,1,EffectContextHandle);
	EffectContextHandle.AddSourceObject(this);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	return ActiveEffectHandle;
}

