// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayer.h"
#include "Components/WidgetComponent.h"
#include "Debug/DebugHelper.h"

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
	AttackType =ATKType;
	ConsumeTurn = bConsumeTurn;
	Melee = bMelee;
	TargetActor = Target;
	RotateToTargetActor = TargetActor;
	TargetLocation = TargetActor->GetActorLocation() + TargetActor->GetActorForwardVector() * 150.f;
	PlayAnimationAndTimeLine();
}

void ABattlePlayer::MultipleAtk(TArray<AActor*> Target, bool bConsumeTurn, bool bMelee, EAttackType ATKType)
{
	AttackType = ATKType;
	ConsumeTurn = bConsumeTurn;
	Melee = bMelee;
	TargetActors = Target;
	if (!TargetActors.IsValidIndex(0)) return;
	if (!TargetActors[0]) return;
	RotateToTargetActor = TargetActors[0];
	TargetLocation = TargetActors[0]->GetActorLocation() + TargetActors[0]->GetActorForwardVector() * 150.f;
	PlayAnimationAndTimeLine();
}

void ABattlePlayer::PlayAnimationAndTimeLine()
{
	RotateToTarget_Timeline.PlayFromStart();
}

void ABattlePlayer::TL_RotateToTarget(float DeltaTime)
{
	FString str = FString::SanitizeFloat(DeltaTime);
	Debug::Print(*str);
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));

	InitializeData();

	//初始化时间轴
	if (Curve_RotateToTarget)
	{
		FOnTimelineFloat RtTHandler;
		RtTHandler.BindUFunction(this,FName("TL_RotateToTarget"));
		RotateToTarget_Timeline.AddInterpFloat(Curve_RotateToTarget, RtTHandler);
	}
}

void ABattlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//注册时间轴
	RotateToTarget_Timeline.TickTimeline(DeltaTime);
}
