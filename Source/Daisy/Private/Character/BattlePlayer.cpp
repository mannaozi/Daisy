// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayer.h"
#include "Components/WidgetComponent.h"
#include "Character/BattleEnemy.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Debug/DebugHelper.h"
#include "Game/DaisyGameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	SetHiddenForPlayer(false);
	if (Melee)
	{
		if (!UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
		{
			//切换视角
			float BlendTime = 0.6f;
			if (AttackType == EAttackType::AT_FollowTK)
			{
				BlendTime = 0.0f;
			}
			UGameplayStatics::GetPlayerController(GetWorld(),0)->SetViewTargetWithBlend(RotateToTargetActor,BlendTime);
		}
		//播放向前运动的动画
		PlaySpecifiedAnim("Slide_F");
		//时间轴移动
		
	}
	else
	{
		
	}
}
float ABattlePlayer::PlaySpecifiedAnim(FString Str)
{
	float AnimTime = 0.0f;
	if (playerAtr.Montages.Contains(Str))
	{
		AnimTime = PlayAnimMontage(*(playerAtr.Montages.Find(Str)));
	}
	return AnimTime;
}
void ABattlePlayer::TL_RotateToTarget(float DeltaTime)
{
	FString str = FString::SanitizeFloat(DeltaTime);
	Debug::Print(*str);
	if (!RotateToTargetActor) return;
	if (!Cast<ABattleEnemy>(RotateToTargetActor)) return;
	FRotator TargetRotator = (RotateToTargetActor->GetActorLocation() - GetActorLocation()).Rotation();
	FRotator TempRotator = UKismetMathLibrary::RLerp(OriginRotation,TargetRotator,DeltaTime,true);
	SetActorRotation(FRotator(0,TempRotator.Yaw,0));
}

void ABattlePlayer::SetHiddenForPlayer(bool bCustomHidden)
{
	SetActorHiddenInGame(bCustomHidden);
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));

	InitializeData();

	//初始镜头角度
	float SpringArmYaw = 0.0f;
	if (positionID != -1)
	{
		switch (positionID)
		{
			case 0:
				break;
			case 1:
				SpringArmYaw = -40.0f;
				break;
			case 2:
				SpringArmYaw = -30.0f;
				break;
			case 3:
				SpringArmYaw = -20.0f;
				break;
			case 4:
				SpringArmYaw = -10.0f;
				break;
		}
		GetCameraBoom()->SetRelativeRotation(FRotator(0.0f, SpringArmYaw, 0.0f));
	}
	
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
