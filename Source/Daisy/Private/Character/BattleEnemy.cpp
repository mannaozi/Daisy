// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleEnemy.h"
#include "Actors/FloatingInicator.h"
#include "Character/BattlePlayer.h"
#include "Components/WidgetComponent.h"
#include "Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
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

void ABattleEnemy::HandleFX()
{
	EnterDialtion();
	PlayerCameraShake();
}

void ABattleEnemy::EnterDialtion()
{
	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) > 0.9f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),0.01f);
		GetWorld()->GetTimerManager().SetTimer(DilationHandle,this,&ABattleEnemy::ResetDilation,0.0005f,false);
	}
}

void ABattleEnemy::ResetDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1.f);
}

void ABattleEnemy::PlayerCameraShake()
{
	ABattlePlayer* Player = Cast<ABattlePlayer>(DmgCauser);
	if (Player == nullptr) return;
	EAttackType AttackType = Player->AttackType;
	TSubclassOf<UCameraShakeBase> CSB = nullptr;
	switch (AttackType)
	{
	case EAttackType::AT_EMAX:
		CSB = nullptr;
		break;
	case EAttackType::AT_NormalATK:
		CSB = NormalCS;
		break;
	case EAttackType::AT_SkillATK:
		CSB = SkillCS;
		break;
	case EAttackType::AT_FollowTK:
		CSB = FollowCS;
		break;
	case EAttackType::AT_Ultimate:
		CSB = UltimateCS;
		break;
	case EAttackType::AT_DelayATK_E:
		CSB = nullptr;
		break;
	}
	if (CSB == nullptr) return;
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
	PlayWorldCameraShake(GetWorld(),CSB,GetActorLocation(),0.0f,4000.f,0.0f,false);
}

bool ABattleEnemy::HandleToughness(float ReceivedT, ECombatType CauserCType)
{
	bool Hit = CheckElementATK(CauserCType);
	if (Hit)
	{
		CurThoughness = CurThoughness - ReceivedT;
		return true;
	}
	else
	{
		return false;
	}
}

bool ABattleEnemy::CheckElementATK(ECombatType CauserCType)
{
	bool DamageToughness = false;
	for (auto ArrayElem : Weaknesses)
	{
		if (ArrayElem == CauserCType)
		{
			DamageToughness = true;
			return DamageToughness;
		}
	}
	return false;
}

void ABattleEnemy::HandleIndicatorNums(FVector Location, float FloatingNum)
{
	FTransform CustomTransform;
	CustomTransform.SetLocation(Location);
	CustomTransform.SetRotation(FQuat(0,0,0,0));
	CustomTransform.SetScale3D(FVector(1,1,1));
	AFloatingInicator* FloatingInicator = GetWorld()->SpawnActorDeferred<AFloatingInicator>(FloatingIndicatorClass,CustomTransform);
	FloatingInicator->FloatingNum = FloatingNum;
	FloatingInicator->SpecifiedColor = FColor::Yellow;
	FloatingInicator->CurrentLocation = CustomTransform.GetLocation();
	FloatingInicator->FinishSpawning(CustomTransform);
}

float ABattleEnemy::PlaySpecificAnim(const FString& AnimKey)
{
	if (!AnimMontages.Contains(AnimKey)) return 0;
	if (*(AnimMontages.Find(AnimKey)) == nullptr) return 0;
	GetMesh()->GetAnimInstance()->StopAllMontages(0);
	float AnimTime = PlayAnimMontage(*(AnimMontages.Find(AnimKey)));
	
	return AnimTime;
}

void ABattleEnemy::EnterStun(int32 DelayTurns)
{
	bStun = true;
	PlaySpecificAnim("Stun");
	//行动推迟25%
	ActionValue = ActionValue + (10000 / EnemyAtr.SPD) * 0.25;

	//设置stun回合
	RecoverFromStunTurns = DelayTurns;
	
	PlayStunVFX();
	
	ExtraActionWhenStun(true);

	if (bDelayed_ATK)
	{
		SetDelayedTarget(false,DelayedTarget);
	}
}

void ABattleEnemy::PlayStunVFX()
{
	if (StunVFXComp != nullptr) return;
	FVector Location = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + StunVFXHeight);
	StunVFXComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),StunVFX,Location);
}

void ABattleEnemy::SetDelayedTarget(bool Delay, ABattlePlayer* Target)
{
	//
}

void ABattleEnemy::RecoverFromStun()
{
	RecoverFromStunTurns -= 1;
	if (RecoverFromStunTurns > 0) return;

	StopAnimMontage();
	RecoverFromStunTurns = 0;
	CurThoughness = MaxThoughness;
	UpdateHeadBar();
	bStun = false;
	ExtraActionWhenStun(false);
	if (StunVFXComp != nullptr)
	{
		StunVFXComp->DestroyComponent();
		StunVFXComp = nullptr;
	}
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
	//FString str = FString::SanitizeFloat(HP_Dmg);
	//Debug::Print(*str);
	DmgCauser = causer;
	
	ABattlePlayer* Player = Cast<ABattlePlayer>(DmgCauser);
	if (Player == nullptr) return;
	const bool HitWeakness = HandleToughness(Toughness_Dmg,Player->playerAtr.CombatType);

	float HpDmgMulti = 1.0f;
	if (HitWeakness)
	{
		HpDmgMulti = 1.0f;
	}
	else
	{
		HpDmgMulti = 0.8f;
	}

	float HitStunMulti = 0.9f;
	if (bStun)
	{
		HitStunMulti = 1.0f;
	}
	else
	{
		HitStunMulti = 0.9f;
	}
	ReceivedHPDmg = HP_Dmg * HitStunMulti * HpDmgMulti * 1.0f;

	CurHP = CurHP - ReceivedHPDmg;

	HandleIndicatorNums(GetActorLocation(),ReceivedHPDmg);

	UpdateHeadBar();

	HandleFX();

	//是否死亡
	if (CurHP <= 0)
	{
		// TBD - 死亡逻辑
		bDead = true;

		PlaySpecificAnim("Die");

		HeadBar->SetVisibility(false);

		ExtraActionWhenStun(true);

		// 如果该组件有效，则删除
		if (StunVFXComp != nullptr)
		{
			StunVFXComp->DestroyComponent();
			StunVFXComp = nullptr;
		}

		// 将死亡事件派发给Player，添加EP（能量）
		OnEnemyDeath.Broadcast(this, DmgCauser);
	}
	else
	{
		if (CurThoughness <= 0)
		{
			if (bStun) return;
			EnterStun(1);
		}
		else
		{
			FString str;
			bool bRandomBool = FMath::RandHelper(100) > 50;
			if (bRandomBool)
			{
				str = "Hit1";
			}
			else
			{
				str = "Hit2";
			}
			PlaySpecificAnim(str);
		}
	}
}

void ABattleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	EnemyAtr = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));
	
	InitializeData();
}
