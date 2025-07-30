// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DaisyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class DAISY_API UDaisyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UDaisyAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//Attribute
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Health);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxHealth,Category="Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,MaxHealth);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Shield,Category="Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Shield);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Energy,Category="Attributes")
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Energy);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxEnergy,Category="Attributes")
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,MaxEnergy);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Attack,Category="Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Attack);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Defense,Category="Attributes")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Defense);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Critical,Category="Attributes")
	FGameplayAttributeData Critical;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Critical);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_CriticalDamage,Category="Attributes")
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,CriticalDamage);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Speed,Category="Attributes")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Speed);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Level,Category="Attributes")
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Level);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);
	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldEnergy);
	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack);
	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldDefense);
	UFUNCTION()
	void OnRep_Critical(const FGameplayAttributeData& OldCritical);
	UFUNCTION()
	void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage);
	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel);
};
