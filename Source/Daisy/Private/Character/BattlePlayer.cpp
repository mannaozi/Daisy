// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayer.h"

void ABattlePlayer::RefreshActionValueBySpd()
{
	ActionValue = Distance / playerAtr.SPD;
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));

	// 初始化行动值
	RefreshActionValueBySpd();
}
