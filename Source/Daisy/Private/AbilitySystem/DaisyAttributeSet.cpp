// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DaisyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"
#include "GameplayEffect.h"
#include "Character/BattleEnemy.h"
#include "Character/BattlePlayer.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Game/BattleManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

UDaisyAttributeSet::UDaisyAttributeSet()
{
	
}

void UDaisyAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Shield,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Energy,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,MaxEnergy,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Attack,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Defense,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Critical,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,CriticalDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Speed,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Level,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,Toughness,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDaisyAttributeSet,MaxToughness,COND_None,REPNOTIFY_Always);
}

void UDaisyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxHealth());
	}
	if(Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxEnergy());
	}
}

void UDaisyAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	const FGameplayEffectModCallbackData DebugData = Data; 
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth,0.f,GetMaxHealth()));
			const bool bFatal = NewHealth <= 0.f;
			//获取EffectContex中的暴击和颜色
			const bool bCritical = UDaisyBlueprintFunctionLibrary::IsCriticalHit(Props.EffectContextHandle);
			const FColor DamageColor = UDaisyBlueprintFunctionLibrary::GetDamageColor(Props.EffectContextHandle);
			//将伤害值加到最终伤害上
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->Damage_End += LocalIncomingDamage;
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->UpdateDamage(true);
			if (!bFatal)
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FDaisyGameplayTags::Get().HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
				if (ABattleEnemy* Enemy = Cast<ABattleEnemy>(Props.TargetCharacter))
				{
					Enemy->HandleIndicatorNums(Enemy->GetActorLocation(),LocalIncomingDamage,bCritical,DamageColor);
				}
				if (ABattlePlayer* Player = Cast<ABattlePlayer>(Props.TargetCharacter))
				{
					Player->SpawnFloatingAndPlaySound(Player->GetActorLocation(),LocalIncomingDamage,FColor::Red);
				}
			}
			else
			{
				if (ABattleEnemy* Enemy = Cast<ABattleEnemy>(Props.TargetCharacter))
				{
					Enemy->HandleIndicatorNums(Enemy->GetActorLocation(),LocalIncomingDamage,bCritical,DamageColor);
					Enemy->Die();
				}
				//玩家死亡
				if (ABattlePlayer* Player = Cast<ABattlePlayer>(Props.TargetCharacter))
				{
					Player->Die();
				}
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetToughnessAttribute())
	{
		if (GetToughness()<=0.f)
		{
			ABattleEnemy* Enemy = Cast<ABattleEnemy>(Props.TargetCharacter);
			if (Enemy) Enemy->EnterStun(1);
		}
	}
	if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		SetEnergy(FMath::Clamp(GetEnergy(),0.f,GetMaxEnergy()));
		if (GetEnergy() == GetMaxEnergy())
		{
			ABattlePlayer* Player = Cast<ABattlePlayer>(Props.TargetCharacter);
			if (Player) Player->bCanUltimate = true;
		}
	}
}

void UDaisyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Health, OldHealth);
}

void UDaisyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, MaxHealth, OldMaxHealth);
}

void UDaisyAttributeSet::OnRep_Toughness(const FGameplayAttributeData& OldToughness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Toughness, OldToughness);
}
void UDaisyAttributeSet::OnRep_MaxToughness(const FGameplayAttributeData& OldMaxToughness) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, MaxToughness, OldMaxToughness);
}
void UDaisyAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Shield, OldShield);
}

void UDaisyAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldEnergy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Energy, OldEnergy);
}

void UDaisyAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, MaxEnergy, OldMaxEnergy);
}

void UDaisyAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Attack, OldAttack);
}

void UDaisyAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Defense, OldDefense);
}

void UDaisyAttributeSet::OnRep_Critical(const FGameplayAttributeData& OldCritical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Critical, OldCritical);
}

void UDaisyAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, CriticalDamage, OldCriticalDamage);
}

void UDaisyAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Speed, OldSpeed);
}

void UDaisyAttributeSet::OnRep_Level(const FGameplayAttributeData& OldLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDaisyAttributeSet, Level, OldLevel);
}

void UDaisyAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)
	// ！！！Controller为空
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}
