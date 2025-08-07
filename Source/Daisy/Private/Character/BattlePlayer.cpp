// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattlePlayer.h"

#include "DaisyGameplayTags.h"
#include "AbilitySystem/DaisyAbilitySystemComponent.h"
#include "AbilitySystem/DaisyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Character/BattleEnemy.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Debug/DebugHelper.h"
#include "Game/BattleManager.h"
#include "Game/DaisyGameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/FloatingInicator.h"
#include "daisy/daisyEnum.h"

ABattlePlayer::ABattlePlayer()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDaisyAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UDaisyAttributeSet>("AttributeSet");

	
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
	UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet);
	ActionValue = Distance / DaisyAS->GetSpeed();
}

void ABattlePlayer::SetATK(EAttackType ATKType, int32 AttackCountInOneCycle)
{
	attackCountInOneCycle = AttackCountInOneCycle;
	bool bCastingBuff = playerAtr.BuffSkillStats.Contains(ATKType);
	if (bCastingBuff)
	{
		BuffInfo = *(playerAtr.BuffSkillStats.Find(ATKType));
	}
	else
	{
		BuffInfo.BuffType = EBuffTypes::BT_EMAX;
		BuffInfo.BuffRatio = 0.0f;
	}
	if (!playerAtr.MultipleTargets.Contains(ATKType)) return;
	if (*(playerAtr.MultipleTargets.Find(ATKType)))
	{
		//多人
		for (auto ArrayElem : TargetActors)
		{
			ICombatInterface* Interface = Cast<ICombatInterface>(ArrayElem);
			if (Interface)
			{
				float hpdmg;
				float tdmg;
				CalculateDmg(bCastingBuff,hpdmg,tdmg);
				Interface->HitHandle(this,hpdmg,tdmg,BuffInfo);
			}
		}
	}
	else
	{
		ICombatInterface* Interface = Cast<ICombatInterface>(TargetActor);
		if (Interface)
		{
			float hpdmg;
			float tdmg;
			CalculateDmg(bCastingBuff,hpdmg,tdmg);
			Interface->HitHandle(this,hpdmg,tdmg,BuffInfo);
		}
	}
}

void ABattlePlayer::HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
	// 受击逻辑
	DmgCauser = causer;
	receivedDmg = HP_Dmg;
	float l_RealRecievedDmg = HP_Dmg;
	EBuffTypes BuffType = buff_Info.BuffType;
	switch (BuffType)
	{
	case EBuffTypes::BT_EMAX:
		
		// 来自敌人攻击时再考虑防御力；增益魔法不考虑防御力
			l_RealRecievedDmg = HP_Dmg * (200.0f / (200.0f + playerAtr.DEF));
		
		// 处理盾值和血量
		HandleShieldAndHP(l_RealRecievedDmg);

		// 判断播放受击动画还是击败动画（可复活）
		if (CurHp <= 0.0f)
		{
			bDead = true;
			PlaySpecifiedAnim("Die");
		}
		else
		{
			PlaySpecifiedAnim("Hit_F");
			// 每受击1次增加能量，设定增加5点
			HandleEP(EAttackType::AT_EMAX, true, 5.0f);
		}
		break;
	case EBuffTypes::BT_Shield:
		// 盾值不可叠加，但重复被释放可刷新持续时间
			AddShieldBuff(receivedDmg);
			break;
	case EBuffTypes::BT_Heal:
		//治疗
			Healing(l_RealRecievedDmg);	
			break;
	case EBuffTypes::BT_Resurrection:
		//复活逻辑
			Resurrection();
			break;
	case EBuffTypes::BT_MoveForward:
		// 行动提前（拉条）
			MoveForward();
			break;
	default:
		break;
	}
}

void ABattlePlayer::CountBuffsTimer()
{
	CheckShieldModifier();
}

void ABattlePlayer::TryFollowingATK()
{
	// 检查是否可动作
	ABattlePlayer* l_playerChar = Cast<ABattlePlayer>(shieldGuard);
	if (l_playerChar != nullptr)
	{
		if (!l_playerChar->bStun && !l_playerChar->bDead)
		{
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->ActivePlayerRef = l_playerChar;
			// 执行追加攻击动画；传入攻击对象DmgCauser
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->StartFollowingATK(DmgCauser);

			// 记得跳出，等到追加攻击完成后，再进入下一个动作
			return;
		}
	}

	// 结束回合，不消耗回合数
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->B3_TurnEnd(nullptr, false);
}

void ABattlePlayer::Healing(float val)
{
	// 生成特效
	FVector l_loc = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 80.0f);
	CurHp = FMath::Clamp(CurHp + val, 0.0f, MaxHP);
	SpawnFloatingAndPlaySound(GetActorLocation(), val, FColor::Green);
}

void ABattlePlayer::AddShieldBuff(float val)
{
	// 给玩家角色属性刷新临时盾值
	CurShield = MaxShield = val;
	
	// 设置持续回合数
	shieldDuration = 1;

	// 尝试删除盾的标签后再加上
	Tags.Remove(*shieldTag);
	Tags.Add(*shieldTag);

	// 明确发动追加攻击的对象
	shieldGuard = DmgCauser;

	// 弹出盾值数字和播放声音
	SpawnFloatingAndPlaySound(GetActorLocation(), receivedDmg, FColor::Cyan);

}

void ABattlePlayer::CheckShieldModifier()
{
	if (shieldDuration != 0)
	{
		--shieldDuration;
	}
	else
	{
		// 重置
		shieldDuration = 0;
		CurShield = 0.0f;
		MaxShield = 0.0f;
		Tags.Remove(*shieldTag);
		shieldGuard = nullptr;
	}
}

void ABattlePlayer::Resurrection()
{
	bDead = false;

	StopAnimMontage();

	// 仅1点血
	CurHp = 1.0f;

	RefreshActionValueBySpd();
}

void ABattlePlayer::MoveForward()
{
	ActionValue -= 50;
	ActionValue = FMath::Max(0.0f, ActionValue);
}

void ABattlePlayer::HandleShieldAndHP(float dmg)
{
	// 有套盾先扣盾值，盾值不够再扣血，无套盾直接扣血
	if (CurShield > 0.0f)
	{
		CurShield = CurShield - dmg;

		// 生成白色数字
		SpawnFloatingAndPlaySound(GetActorLocation(), dmg, FColor::White);

		// 扣除后，是否还有剩余盾值
		if (CurShield > 0.0f)
		{
			// 如有，释放套盾技能的玩家角色如果也存活，则追加攻击
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->bTryFollowingATK = true;
			UDaisyBlueprintFunctionLibrary::FindBattleManager()->GuardedChar = this;
		}
		else
		{
			// 如果没有，则把curThougness不足的部分加算到血量上
			// curToughness <= 0;
			CurHp = CurHp + CurShield;
			
			// 生成红色数字
			SpawnFloatingAndPlaySound(GetActorLocation(), CurShield * (-1.0f), FColor::Red);
			// 播放血量减少时的受击声音
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());

			// 重置盾值相关的变量值
			CurShield = 0.0f;
			shieldDuration = 0;
			MaxShield = 0.0f;
			Tags.Remove(*shieldTag);

		}
	}
	else
	{
		CurHp = FMath::Clamp(CurHp - dmg, 0.0f, MaxHP);

		// 生成红色数字
		SpawnFloatingAndPlaySound(GetActorLocation(), dmg, FColor::Red);

		// 播放血量减少时的受击声音
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSFX, GetActorLocation());
	}
}

void ABattlePlayer::SpawnFloatingAndPlaySound(FVector loc, float num, FColor col)
{
	// 需传入显示的数字，故使用延迟生成
	FTransform CustomTransform;
	CustomTransform.GetLocation() = loc;
	CustomTransform.GetRotation() = FQuat(0, 0, 0, 0);
	CustomTransform.GetScale3D() = FVector(1, 1, 1);
	AFloatingInicator* l_FI3 = GetWorld()->SpawnActorDeferred<AFloatingInicator>(
		FloatingIndicatorClass, CustomTransform);
	l_FI3->FloatingNum = num;
	l_FI3->SpecifiedColor = col;
	l_FI3->CurrentLocation = GetActorLocation();
	l_FI3->FinishSpawning(CustomTransform);
}

void ABattlePlayer::InitializeData()
{
	//初始化数据
	bPlayerFaction = true;
	AvatarIcon = playerAtr.CharIcon_Banner;
	MaxHP = playerAtr.HP;
	CurHp = MaxHP;
	MaxEnergy = playerAtr.Energy;
	CurEnergy = 0.0f;
	MaxShield = 0.0f;
	CurShield = 0.0f;
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
	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, GetActorLocation().Z);
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
	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, GetActorLocation().Z);
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
		Sliding_F_Timeline.PlayFromStart();
		//Sliding_B_Timeline.PlayFromStart();
	}
	else
	{
		PlayATKAnimByATKType();
	}
}

void ABattlePlayer::GetPlayerAttributes(float& mHP, float& cHP, float& mEP, float& cEP, float& mT, float& cT)
{
	UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet);
	mHP = DaisyAS->GetMaxHealth();
	cHP = DaisyAS->GetHealth();
	mEP = MaxEnergy;
	cEP =  CurEnergy;
	mT = MaxShield;
	cT = CurShield;
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
	//FString str = FString::SanitizeFloat(DeltaTime);
	//Debug::Print(*str);
	if (!RotateToTargetActor) return;
	if (!Cast<ABattleEnemy>(RotateToTargetActor)) return;
	FRotator TargetRotator = (RotateToTargetActor->GetActorLocation() - GetActorLocation()).Rotation();
	FRotator TempRotator = UKismetMathLibrary::RLerp(OriginRotation,TargetRotator,DeltaTime,true);
	SetActorRotation(FRotator(0,TempRotator.Yaw,0));
}

void ABattlePlayer::TL_SlideF(float DeltaTime)
{
	//移动玩家角色
	FVector targetLocation = UKismetMathLibrary::VLerp(OriginLocation,TargetLocation,DeltaTime);
	SetActorLocation(targetLocation);
}

void ABattlePlayer::TL_SlideF_Finished()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.2f);
	GetWorldTimerManager().SetTimer(PlayATKAnimHandle,this,&ABattlePlayer::PlayATKAnimByATKType,0.2f,false);
}

void ABattlePlayer::TL_SlideB(float DeltaTime)
{
	FVector targetLocation = UKismetMathLibrary::VLerp(GetActorLocation(),OriginLocation,DeltaTime);
	SetActorLocation(targetLocation);
}

void ABattlePlayer::TL_SlideB_Finished()
{
	//0.4秒后结束回合
	GetWorldTimerManager().SetTimer(MeleePlayerEndHandle,this,&ABattlePlayer::GeneralPlayerAttackOver,0.4f,false);
}

void ABattlePlayer::SetHiddenForPlayer(bool bCustomHidden)
{
	SetActorHiddenInGame(bCustomHidden);
}

void ABattlePlayer::PlayATKAnimByATKType()
{
	//根据攻击类型播放Montage
	FString SpecifiedActionString;
	switch (AttackType)
	{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		SpecifiedActionString = "NormalATK";
		break;
	case EAttackType::AT_SkillATK:
		SpecifiedActionString = "SkillATK";
		break;
	case EAttackType::AT_FollowTK:
		SpecifiedActionString = "FollowATK";
		break;
	case EAttackType::AT_Ultimate:
		SpecifiedActionString = "UltimateATK";
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	}
	FGameplayTag ActiveTag = UDaisyBlueprintFunctionLibrary::FindBattleManager()->GetTagByName(SpecifiedActionString);
	
	//float AnimTime = PlaySpecifiedAnim(SpecifiedActionString);
	Cast<UDaisyAbilitySystemComponent>(GetAbilitySystemComponent())->ActiveAbilityByTag(ActiveTag);
	//能量处理)
	HandleEP(AttackType,false,0.0f);
	
	if (Melee)
	{
		//播放动画后执行下一逻辑
		//GetWorldTimerManager().SetTimer(PlayATKAnimHandle,this,&ABattlePlayer::AfterPlayingMeleeATKAnim,AnimTime,false);
	}
	else
	{
		//GetWorldTimerManager().SetTimer(PlayATKAnimHandle,this,&ABattlePlayer::GeneralPlayerAttackOver,AnimTime,false);
	}
}

void ABattlePlayer::AfterPlayingMeleeATKAnim()
{
	//播放回去的montage
	PlaySpecifiedAnim("Slide_B");
	Sliding_B_Timeline.PlayFromStart();
}

void ABattlePlayer::GeneralPlayerAttackOver()
{
	//恢复玩家Rotation
	RotateToTarget_Timeline.ReverseFromEnd();
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->B3_TurnEnd(this,ConsumeTurn);
}

void ABattlePlayer::HandleEP(EAttackType ATKType, bool bDirect, float val)
{
	float deltaEP = 0.0f;
	switch (AttackType)
	{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		deltaEP = 20.f;
		break;
	case EAttackType::AT_SkillATK:
		deltaEP = 30.f;
		break;
	case EAttackType::AT_FollowTK:
		deltaEP = 10.f;
		break;
	case EAttackType::AT_Ultimate:
		deltaEP = 5.f;
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	}
	if (bDirect) {deltaEP = CurEnergy + val;}
	else
	{
		CurEnergy = CurEnergy + deltaEP * 1.0f;
	}
}

void ABattlePlayer::CalculateDmg(bool Buff, float& hpDmg, float& toughnessDmg)
{
	float f_hpDmg = 0.0f;
	float f_toughnessDmg = 0.0f;
	float f_ratio = 0.0f;
	
	switch (AttackType)
	{
	case EAttackType::AT_EMAX:
		f_ratio = 0.0f;
		break;
	case EAttackType::AT_NormalATK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_SkillATK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_FollowTK:
		f_ratio = 1.0f;
		break;
	case EAttackType::AT_Ultimate:
		f_ratio = playerAtr.UltimateRatio;
		break;
	case EAttackType::AT_DelayATK_E:
		f_ratio = 1.0f;
		break;
	default:
		f_ratio = 0.0f;
		break;
	}
	
	if (Buff)
	{
		f_hpDmg = playerAtr.ATK / float(attackCountInOneCycle) * f_ratio * BuffInfo.BuffRatio;
	}
	else
	{
		f_hpDmg = playerAtr.ATK / float(attackCountInOneCycle) * f_ratio;
	}

	f_toughnessDmg = playerAtr.ATK / float(attackCountInOneCycle) * f_ratio;
	
	hpDmg = f_hpDmg;
	toughnessDmg = f_toughnessDmg;
	
}

void ABattlePlayer::SetDelayedMark(bool bNewVisibility)
{
	MarkedIcon->bHiddenInGame = !bNewVisibility;
}

void ABattlePlayer::InitValue()
{
	if (const UDaisyAttributeSet* AuraAS = Cast<UDaisyAttributeSet>(AttributeSet))
	{
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
		OnShieldChanged.Broadcast(AuraAS->GetShield());
		OnEnergyChanged.Broadcast(AuraAS->GetEnergy());
		OnMaxEnergyChanged.Broadcast(AuraAS->GetMaxEnergy());
	}
}

void ABattlePlayer::BindAttributeDelegate()
{
	if (const UDaisyAttributeSet* AuraAS = Cast<UDaisyAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetShieldAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnShieldChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetEnergyAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnEnergyChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxEnergyAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxEnergyChanged.Broadcast(Data.NewValue);
			}
		);
	}
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	playerAtr = *(PlayerCharsDT->FindRow<FPlayerCharAttributes>(DataRow, s, true));
	
	InitAbilityActorInfo();
	InitializeDefaultAttributes();
	AddCharacterAbilities();
	InitializeData();
	BindAttributeDelegate();
	
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
	if (Curve_Sliding)
	{
		FOnTimelineFloat SLFHandler;
		SLFHandler.BindUFunction(this,FName("TL_SlideF"));
		Sliding_F_Timeline.AddInterpFloat(Curve_Sliding, SLFHandler);

		FOnTimelineEvent SLFEventHandler;
		SLFEventHandler.BindUFunction(this,FName("TL_SlideF_Finished"));
		Sliding_F_Timeline.SetTimelineFinishedFunc(SLFEventHandler);

		FOnTimelineFloat SLBHandler;
		SLBHandler.BindUFunction(this,FName("TL_SlideB"));
		Sliding_B_Timeline.AddInterpFloat(Curve_Sliding, SLBHandler);

		FOnTimelineEvent SLBEventHandler;
		SLBEventHandler.BindUFunction(this,FName("TL_SlideB_Finished"));
		Sliding_B_Timeline.SetTimelineFinishedFunc(SLBEventHandler);
	}
}

void ABattlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//注册时间轴
	RotateToTarget_Timeline.TickTimeline(DeltaTime);
	Sliding_F_Timeline.TickTimeline(DeltaTime);
	Sliding_B_Timeline.TickTimeline(DeltaTime);
}
