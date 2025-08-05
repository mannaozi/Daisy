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
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

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
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	//Attribute
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Health);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxHealth,Category="Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,MaxHealth);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Toughness,Category="Attributes")
	FGameplayAttributeData Toughness;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,Toughness);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_MaxToughness,Category="Attributes")
	FGameplayAttributeData MaxToughness;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,MaxToughness);
	
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

	UPROPERTY(BlueprintReadOnly,Replicated,Category="Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UDaisyAttributeSet,IncomingDamage);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Toughness(const FGameplayAttributeData& OldToughness) const;
	UFUNCTION()
	void OnRep_MaxToughness(const FGameplayAttributeData& OldMaxToughness) const;
	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) const;
	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldEnergy) const;
	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy) const;
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack) const;
	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldDefense) const;
	UFUNCTION()
	void OnRep_Critical(const FGameplayAttributeData& OldCritical) const;
	UFUNCTION()
	void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const;
	UFUNCTION()
	void OnRep_Speed(const FGameplayAttributeData& OldSpeed) const;
	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel) const;
private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};
