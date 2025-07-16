// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleEnemy.h"


void ABattleEnemy::UpdateLockIcon(bool bHide)
{
	
}

void ABattleEnemy::RefreshActionValueBySpd()
{
	ActionValue = Distance / enemyInfo.SPD;
}

void ABattleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	enemyInfo = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));

	// 初始化行动值
	RefreshActionValueBySpd();
}
