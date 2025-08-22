// Fill out your copyright notice in the Description page of Project Settings.


#include "DaisyGameplayTags.h"
#include "GameplayTagsManager.h"

FDaisyGameplayTags FDaisyGameplayTags::GameplayTags;

void FDaisyGameplayTags::InitializeNativeGameplayTags()
{
	//Attribute
	
	GameplayTags.Attribute_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Health"),
		FString("生命值")
	);
	
	GameplayTags.Attribute_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.MaxHealth"),
		FString("最大生命值")
	);

	GameplayTags.Attribute_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Shield"),
		FString("护盾")
	);

	GameplayTags.Attribute_Energy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Energy"),
		FString("能量")
	);
	
	GameplayTags.Attribute_MaxEnergy = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.MaxEnergy"),
		FString("能量上限")
	);
	
	GameplayTags.Attribute_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Attack"),
		FString("攻击力")
	);
	
	GameplayTags.Attribute_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Defense"),
		FString("防御力")
	);

	GameplayTags.Attribute_Critical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Critical"),
		FString("暴击率")
	);
	
	GameplayTags.Attribute_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.CriticalDamage"),
		FString("暴击伤害")
	);
	
	GameplayTags.Attribute_Speed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Speed"),
		FString("速度")
	);

	GameplayTags.Attribute_Level = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Level"),
		FString("等级")
	);

	//Damage Type
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
	FString("Damage")
	);
	
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("火伤")
	);

	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("雷伤")
	);

	GameplayTags.Damage_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Ice"),
		FString("冰伤")
	);
	
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("物理伤害")
	);

	GameplayTags.Damage_Wind = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Wind"),
		FString("风伤")
	);
	
	GameplayTags.Damage_Quantum = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Quantum"),
		FString("量子")
	);
	
	GameplayTags.Damage_Imaginary = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Imaginary"),
		FString("虚数")
	);

	//AttackType
	
	GameplayTags.AttackType_NormalAtk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AttackType.NormalAtk"),
		FString("普通攻击")
	);
	
	GameplayTags.AttackType_SkillAtk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AttackType.SkillAtk"),
		FString("技能攻击")
	);
	
	GameplayTags.AttackType_UltimateAtk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AttackType.UltimateAtk"),
		FString("大招攻击")
	);
	
	GameplayTags.AttackType_FollowAtk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AttackType.FollowAtk"),
		FString("追加攻击")
	);
	
	GameplayTags.AttackType_CounterAtk = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("AttackType.CounterAtk"),
		FString("防御反击")
	);
	
	//通用
	
	GameplayTags.HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("HitReact"),
		FString("受击")
	);
	
	GameplayTags.Death = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Death"),
		FString("死亡")
	);

	//Montage
	GameplayTags.Ability_Montage_Remote = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Montage.Remote"),
		FString("远程攻击")
	);

	GameplayTags.Ability_Montage_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Montage.Melee"),
		FString("近战攻击")
	);

	//Enemy Attack
	GameplayTags.EnemyAttack_ATK1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("EnemyAttack.ATK1"),
		FString("敌人攻击")
	);
	
	GameplayTags.EnemyAttack_ATK2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("EnemyAttack.ATK2"),
		FString("敌人攻击")
	);
	
	GameplayTags.EnemyAttack_ATK3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("EnemyAttack.ATK3"),
		FString("敌人攻击")
	);
	
	GameplayTags.Buff_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.Shield"),
		FString("加护盾的Buff")
	);
	
	GameplayTags.Buff_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.Attack"),
		FString("加攻击的Buff")
	);

	GameplayTags.Buff_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.Defense"),
		FString("加防御的Buff")
	);
	
	GameplayTags.Buff_Critical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.Critical"),
		FString("加暴击率的Buff")
	);
	
	GameplayTags.Buff_CriticalDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.CriticalDamage"),
		FString("加暴击伤害的Buff")
	);
	
	GameplayTags.Buff_Speed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Buff.Speed"),
		FString("加速度的Buff")
	);
	
	//PlayerState
	GameplayTags.Player_State_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.Dodge"),
		FString("角色处于闪避状态")
	);

	GameplayTags.Player_State_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.Defense"),
		FString("角色处于格挡状态")
	);

	GameplayTags.Player_State_Break = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.Break"),
		FString("打断技能")
	);
	
	GameplayTags.Player_State_DodgeEnd = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.DodgeEnd"),
		FString("闪避结束")
	);

	GameplayTags.Player_State_DodgeEnd = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.DodgeEnd"),
		FString("闪避结束")
	);
	
	GameplayTags.Player_State_ExtremeDodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.ExtremeDodge"),
		FString("极限闪避")
	);
	
	GameplayTags.Player_State_ExtremeDodge = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.ExtremeDodge"),
		FString("极限闪避")
	);
	
	GameplayTags.Player_State_CounterAtkEnd = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.State.CounterAtkEnd"),
		FString("防御反击结束")
	);
}
