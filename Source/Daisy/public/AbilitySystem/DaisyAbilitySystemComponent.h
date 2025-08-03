// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DaisyAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void ActiveAbilityByTag(const FGameplayTag& Tag);
};
