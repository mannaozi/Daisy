// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DaisyAttributeSet.h"

#include "Net/UnrealNetwork.h"

UDaisyAttributeSet::UDaisyAttributeSet()
{
	InitHealth(1000.f);
	InitSpeed(200.f);
}

void UDaisyAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Shield,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Energy,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,MaxEnergy,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Attack,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Defense,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Critical,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,CriticalDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Speed,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Level,COND_None,REPNOTIFY_Always);
}

void UDaisyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Health, OldHealth);
}

void UDaisyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, MaxHealth, OldMaxHealth);
}

void UDaisyAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Shield, OldShield);
}

void UDaisyAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Energy, OldEnergy);
}

void UDaisyAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, MaxEnergy, OldMaxEnergy);
}

void UDaisyAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Attack, OldAttack);
}

void UDaisyAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Defense, OldDefense);
}

void UDaisyAttributeSet::OnRep_Critical(const FGameplayAttributeData& OldCritical)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Critical, OldCritical);
}

void UDaisyAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, CriticalDamage, OldCriticalDamage);
}

void UDaisyAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Speed, OldSpeed);
}

void UDaisyAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Level, OldLevel);
}