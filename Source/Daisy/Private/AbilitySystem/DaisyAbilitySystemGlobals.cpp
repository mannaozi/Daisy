// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DaisyAbilitySystemGlobals.h"

#include "DaisyAbilityTypes.h"

FGameplayEffectContext* UDaisyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FDaisyGameplayEffectContext;
}
