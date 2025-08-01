// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */

struct FDaisyGameplayTags
{
public:
	static const FDaisyGameplayTags& Get() { return GameplayTags;}
	static void InitializeNativeGameplayTags();
	//Attribute
	FGameplayTag Attribute_Health;
	FGameplayTag Attribute_MaxHealth;
	FGameplayTag Attribute_Shield;
	FGameplayTag Attribute_Energy;
	FGameplayTag Attribute_MaxEnergy;
	FGameplayTag Attribute_Attack;
	FGameplayTag Attribute_Defense;
	FGameplayTag Attribute_Critical;
	FGameplayTag Attribute_CriticalDamage;
	FGameplayTag Attribute_Speed;
	FGameplayTag Attribute_Level;

	//Damage Type
	
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Ice;
	FGameplayTag Damage_Physical;
	FGameplayTag Damage_Wind;
	FGameplayTag Damage_Quantum;
	FGameplayTag Damage_Imaginary;

	//AttackType
	FGameplayTag AttackType_NormalAtk;
	FGameplayTag AttackType_SkillAtk;
	FGameplayTag AttackType_UltimateAtk;
	FGameplayTag AttackType_FollowAtk;

	//通用
	FGameplayTag HitReact;
	FGameplayTag Death;
	
private:
	static FDaisyGameplayTags GameplayTags;
};