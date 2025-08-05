// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DaisyBlueprintFunctionLibrary.generated.h"

struct FGameplayEffectContextHandle;
struct FGameplayEffectSpecHandle;
class ABattleManager;
class UDaisyGameInstance;
/**
 * 
 */
UCLASS()
class DAISY_API UDaisyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure)
	static ABattleManager* FindBattleManager();
	
	UFUNCTION(BlueprintPure)
	static UDaisyGameInstance* GetGameInstance();

	UFUNCTION(BlueprintPure,Category="DaisyAbilitySystemLibrary | GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="DaisyAbilitySystemLibrary | GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool bInCriticalHit);

	UFUNCTION(BlueprintPure,Category="DaisyAbilitySystemLibrary | GameplayEffects")
	static FColor GetDamageColor(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="DaisyAbilitySystemLibrary | GameplayEffects")
	static void SetDamageColor(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,FColor InColor);
};
