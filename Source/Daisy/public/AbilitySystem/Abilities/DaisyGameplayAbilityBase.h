// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DaisyGameplayAbilityBase.generated.h"

class ABattlePlayer;
class ABattleEnemy;
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

	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag AttackTag;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="GE")
	TSubclassOf<UGameplayEffect> InitGameplayEffectClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="GE")
	TSubclassOf<UGameplayEffect> WeakNessGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="GE")
	TSubclassOf<UGameplayEffect> EnergyGameplayEffectClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
	
	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectToEnemy(TArray<ABattleEnemy*> Enemy);

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectToPlayer(TArray<ABattlePlayer*> Player);
	
	UFUNCTION(BlueprintCallable)
	void AddEnergy();

	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle AddBuffToPlayer(ABattlePlayer* Player,TSubclassOf<UGameplayEffect> BuffGameplayEffectClass);
};
