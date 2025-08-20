// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"
#include "DaisyGameplayAbilityBuff.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyGameplayAbilityBuff : public UDaisyGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	TMap<FGameplayTag, FText> TagsToDes;
	
	UFUNCTION(BlueprintCallable)
	FText GetDescriptionByTag(FGameplayTag Tag) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	TMap<FGameplayTag, UTexture2D*> TagsToTex;
	
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetTextureByTag(FGameplayTag Tag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BuffTags")
	TArray<FGameplayTag> BuffTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset")
	TMap<FGameplayTag,float> BuffValues;

	UFUNCTION(BlueprintCallable)
	float GetValueByTag(FGameplayTag Tag) const;
	
	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle AddBuffToPlayer(ABattlePlayer* Player,TSubclassOf<UGameplayEffect> BuffGameplayEffectClass,int32 BuffNum,FGameplayTag BuffTag,float Value);
};
