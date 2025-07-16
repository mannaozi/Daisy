// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleEnemy.h"
#include "Components/WidgetComponent.h"

ABattleEnemy::ABattleEnemy()
{
	HeadBar = CreateDefaultSubobject<UWidgetComponent>("Head Bar");
	HeadBar->SetupAttachment(RootComponent);
	HeadBar->bHiddenInGame = true;
}

void ABattleEnemy::UpdateLockIcon(bool bHide)
{
	LockIcon->bHiddenInGame = bHide;
}

void ABattleEnemy::RefreshActionValueBySpd()
{
	ActionValue = Distance / EnemyAtr.SPD;
}

void ABattleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	EnemyAtr = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));
	
	//初始化数据
	bPlayerFaction = false;
	AvatarIcon = EnemyAtr.CharIcon_Banner;
	// 初始化行动值
	RefreshActionValueBySpd();
}
