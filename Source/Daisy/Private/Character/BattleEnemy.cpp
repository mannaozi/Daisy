// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleEnemy.h"
#include "Components/WidgetComponent.h"
#include "Debug/DebugHelper.h"
#include "UI/HeadBarUI.h"

ABattleEnemy::ABattleEnemy()
{
	HeadBar = CreateDefaultSubobject<UWidgetComponent>("Head Bar");
	HeadBar->SetupAttachment(RootComponent);
	HeadBar->bHiddenInGame = true;
}

void ABattleEnemy::InitializeData()
{
	//初始化数据
	bPlayerFaction = false;
	AvatarIcon = EnemyAtr.CharIcon_Banner;
	CurThoughness = EnemyAtr.Toughness;
	MaxThoughness = EnemyAtr.Toughness;
	CurHP = EnemyAtr.HP;
	MaxHP = EnemyAtr.HP;
	StunVFXHeight = EnemyAtr.StunVFXHeight;
	Weaknesses = EnemyAtr.Weaknesses;
	AnimMontages = EnemyAtr.AnimMontages;
	ValidATKStr = EnemyAtr.ValidATKStr;
	choices = EnemyAtr.Choices;
	originLocation = GetActorLocation();
	// 初始化行动值
	RefreshActionValueBySpd();
	
	HeadBarUI = Cast<UHeadBarUI>(HeadBar->GetUserWidgetObject());
	if (HeadBarUI)
	{
		UpdateHeadBar();
	}
}

void ABattleEnemy::UpdateHeadBar()
{
	HeadBarUI->UpdateEnemyHeadBar(CurHP, CurThoughness, MaxHP, MaxThoughness, Weaknesses);
}

void ABattleEnemy::UpdateLockIcon(bool bHide)
{
	LockIcon->bHiddenInGame = bHide;
}

void ABattleEnemy::RefreshActionValueBySpd()
{
	ActionValue = Distance / EnemyAtr.SPD;
}

void ABattleEnemy::HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo BuffInfo)
{
	if (bDead) return;
	FString str = FString::SanitizeFloat(HP_Dmg);
	Debug::Print(*str);
}

void ABattleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	EnemyAtr = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));
	
	InitializeData();
}
