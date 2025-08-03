// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DaisyGameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag StartupInputTag;
};
