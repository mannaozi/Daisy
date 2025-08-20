// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DaisyGameplayAbilityBuff.h"
#include "Character/BattleEnemy.h"
#include "Character/BattlePlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"
#include "Character/BattleCharacterBase.h"
#include "AbilitySystemComponent.h"


FActiveGameplayEffectHandle UDaisyGameplayAbilityBuff::AddBuffToPlayer(ABattlePlayer* Player,TSubclassOf<UGameplayEffect> BuffGameplayEffectClass,int32 BuffNum,FGameplayTag BuffTag,float Value)
{
	FDaisyGameplayTags GameplayTags = FDaisyGameplayTags::Get();
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* TargetASC = Player->GetAbilitySystemComponent();
	check(EnergyGameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(BuffGameplayEffectClass,1,EffectContextHandle);
	EffectContextHandle.AddSourceObject(this);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle,BuffTag,Value);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	FBuffList List;
	FBuffDescription Description;
	Description.BuffDescription = GetDescriptionByTag(BuffTag);
	Description.CharIcon_Avatar = GetTextureByTag(BuffTag);
	List.Description = Description;
	List.BuffNums = BuffNum;
	List.BuffValue = Value;
	Player->BuffMap.Add(ActiveEffectHandle,List);
	return ActiveEffectHandle;
}

FText UDaisyGameplayAbilityBuff::GetDescriptionByTag(FGameplayTag Tag) const
{
	if (const FText* FoundDescription = TagsToDes.Find(Tag))
	{
		return *FoundDescription;
	}
    
	return FText::GetEmpty();
}

UTexture2D* UDaisyGameplayAbilityBuff::GetTextureByTag(FGameplayTag Tag) const
{
	if (UTexture2D* const* FoundTexture = TagsToTex.Find(Tag))
	{
		return *FoundTexture;
	}
    
	return nullptr;
}

float UDaisyGameplayAbilityBuff::GetValueByTag(FGameplayTag Tag) const
{
	if (const float Value = *BuffValues.Find(Tag))
	{
		return Value;
	}
	return 0;
}