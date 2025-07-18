// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayer.h"
#include "Components/WidgetComponent.h"

ABattlePlayer::ABattlePlayer()
{
	MarkedIcon = CreateDefaultSubobject<UWidgetComponent>("Marked Icon");
	MarkedIcon->SetupAttachment(RootComponent);
	MarkedIcon->bHiddenInGame = true;
}

void ABattlePlayer::UpdateLockIcon(bool bHide)
{
	LockIcon->bHiddenInGame = bHide;
}

void ABattlePlayer::RefreshActionValueBySpd()
{
	ActionValue = Distance / playerAtr.SPD;
}

void ABattlePlayer::InitializeData()
{
	//初始化数据
	bPlayerFaction = true;
	AvatarIcon = playerAtr.CharIcon_Banner;
	MaxHP = playerAtr.HP;
	CurHp = MaxHP;
	MaxEnergy = playerAtr.Energy;
	CurEnergy = MaxEnergy;
	GetMesh()->SetSkeletalMesh(playerAtr.SKM);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(playerAtr.AnimClass);
	OriginLocation = GetActorLocation();
    OriginRotation = GetActorRotation();
	
	// 初始化行动值
	RefreshActionValueBySpd();
}

void ABattlePlayer::SingleAtk(AActor* Target, bool bConsumeTurn, bool bMelee, EAttackType ATKType)
{
	
}

void ABattlePlayer::MultipleAtk(TArray<AActor*> Target, bool bConsumeTurn, bool bMelee, EAttackType ATKType)
{
	
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));

	InitializeData();
}
