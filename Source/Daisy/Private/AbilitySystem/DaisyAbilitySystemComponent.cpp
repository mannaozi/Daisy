// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DaisyAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/DaisyGameplayAbilityBase.h"

void UDaisyAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1.f);
		if(const UDaisyGameplayAbilityBase* AuraAbility = Cast<UDaisyGameplayAbilityBase>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
			/*GiveAbility(AbilitySpec);添加能力不激活 GiveAbilityAndActivateOnce(AbilitySpec);  添加能力立刻激活*/
		}
	}
}

void UDaisyAbilitySystemComponent::ActiveAbilityByTag(const FGameplayTag& Tag)
{
	if(!Tag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(Tag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if(!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}
