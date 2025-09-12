// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleEnemy.h"
#include "Actors/FloatingInicator.h"
#include "Character/BattlePlayer.h"
#include "Components/WidgetComponent.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "UI/HeadBarUI.h"
#include "Game/DaisyGameInstance.h"
#include "Game/BattleManager.h"
#include "AbilitySystem/DaisyAbilitySystemComponent.h"
#include "AbilitySystem/DaisyAttributeSet.h"
#include "Camera/CameraActor.h"

ABattleEnemy::ABattleEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	HeadBar = CreateDefaultSubobject<UWidgetComponent>("Head Bar");
	HeadBar->SetupAttachment(RootComponent);
	HeadBar->bHiddenInGame = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AbilitySystemComponent = CreateDefaultSubobject<UDaisyAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UDaisyAttributeSet>("AttributeSet");
}

void ABattleEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector WidgetLocation = HeadBar->GetComponentLocation();
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
	FRotator NewRotation = (CameraLocation - WidgetLocation).Rotation();
	HeadBar->SetWorldRotation(NewRotation);
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
	totalATK = EnemyAtr.ATK;
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
	HeadBarUI->AddWeakness(WeakNess);
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

void ABattleEnemy::HandleIndicatorNums(FVector Location, float FloatingNum,bool bCriticalHit,FColor DamageColor)
{
	FTransform CustomTransform;
	CustomTransform.SetLocation(Location);
	CustomTransform.SetRotation(FQuat(0,0,0,0));
	CustomTransform.SetScale3D(FVector(1,1,1));
	AFloatingInicator* FloatingInicator = GetWorld()->SpawnActorDeferred<AFloatingInicator>(FloatingIndicatorClass,CustomTransform);
	FloatingInicator->FloatingNum = FloatingNum;
	FloatingInicator->SpecifiedColor = DamageColor;
	FloatingInicator->CurrentLocation = CustomTransform.GetLocation();
	FloatingInicator->bCriticalHit = bCriticalHit;
	FloatingInicator->FinishSpawning(CustomTransform);
}

float ABattleEnemy::PlaySpecificAnim(const FString& AnimKey)
{
	if (!AnimMontages.Contains(AnimKey)) return 0;
	if (*(AnimMontages.Find(AnimKey)) == nullptr) return 0;
	GetMesh()->GetAnimInstance()->StopAllMontages(0);
	//float AnimTime = PlayAnimMontage(*(AnimMontages.Find(AnimKey)));
	FGameplayTag Tag = UDaisyBlueprintFunctionLibrary::FindBattleManager()->GetTagByName_Enemy(AnimKey);
	Cast<UDaisyAbilitySystemComponent>(GetAbilitySystemComponent())->ActiveAbilityByTag(Tag);
	return 0;
}

void ABattleEnemy::EndEnemyTurnFromBP(ABattleEnemy* ActiveActor)
{
	OnEnemyTurnEnd.Broadcast(ActiveActor);
}

void ABattleEnemy::EnterStun(int32 DelayTurns)
{
	bStun = true;
	PlayAnimMontage(StunMontage);
	//行动推迟25%
	UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet);
	ActionValue = ActionValue + (10000 / DaisyAS->GetSpeed()) * 0.25;

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
	// BOSS战时需设置
	bDelayed_ATK = Delay;
	if(Target == nullptr) return;
	// 设置玩家角色的锁定图标
	Target->SetDelayedMark(bDelayed_ATK);

	// 根据锁定的布尔值，重置延迟锁定目标
	if (bDelayed_ATK)
	{
		DelayedTarget = Target;
	}
	else
	{
		DelayedTarget = nullptr;
	}
}

void ABattleEnemy::RecoverFromStun()
{
	RecoverFromStunTurns -= 1;
	if (RecoverFromStunTurns > 0) return;

	StopAnimMontage();
	RecoverFromStunTurns = 0;
	UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet);
	DaisyAS->SetToughness(DaisyAS->GetMaxToughness());
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

EAttackType ABattleEnemy::ActionDecision(const TArray<ABattlePlayer*> playersRef)
{
	ABattleManager* BattleManager = UDaisyBlueprintFunctionLibrary::FindBattleManager();
	TArray<ABattlePlayer*> l_playerRef = playersRef;
	if (DelayedTarget != nullptr)
	{
		if (!DelayedTarget->bDead && bDelayed_ATK)
		{
			// 优先延迟攻击
			actionAnimKey = "DelayedATK";
		}
		else
		{
			actionAnimKey = RandomActionByRatio();
		}
	}
	else
	{
		actionAnimKey = RandomActionByRatio();
	}

	if(actionAnimKey == "None") return EAttackType::AT_EMAX;

	// 是否范围攻击？远程攻击？攻击距离是？
	if(!ValidATKStr.Contains(actionAnimKey)) return EAttackType::AT_EMAX;
	bRadialATK = ValidATKStr.Find(actionAnimKey)->bRadialAction;
	bRangeATK = ValidATKStr.Find(actionAnimKey)->bRangeAction;
	ATKDistance = int32(ValidATKStr.Find(actionAnimKey)->ATK_Distance);

	if (bRadialATK)
	{
		// 范围攻击
		currentTargetsArr = l_playerRef;
		RadialATK(l_playerRef);
		// 如果切换视角，切换至受击的玩家角色视角
		if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
		{
			return EAttackType::AT_SkillATK;
		}
		else
		{
			ACameraActor* TargetCA = BattleManager->RetrieveCamera("tag_c_boss");
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(TargetCA);
			//UDaisyBlueprintFunctionLibrary::FindBattleManager()->SwitchAndHideOtherPlayerChars(true, l_playerRef[0]);
			return EAttackType::AT_SkillATK;
		}	
	}
	else
	{
		if (bDelayed_ATK)
		{
			// 延迟攻击
			currentTarget = DelayedTarget;
			SingleATK(DelayedTarget);
			// 延迟攻击已结束，重置相关bool
			SetDelayedTarget(false, DelayedTarget);
			// 如果切换视角，切换至受击的玩家角色视角
			if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
			{
				return EAttackType::AT_DelayATK_E;
			}
			else
			{
				ACameraActor* TargetCA = BattleManager->RetrieveCamera("tag_c_boss");
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->
				SetViewTargetWithBlend(TargetCA);
				//UDaisyBlueprintFunctionLibrary::FindBattleManager()->SwitchAndHideOtherPlayerChars(true, DelayedTarget);
				return EAttackType::AT_DelayATK_E;
			}		
		}
		else
		{
			// 普通单体攻击时，优先攻击带盾（嘲讽效果）的玩家角色
			// 具体表现为查找特定tag
			ABattlePlayer* l_ShieldPlayerRef = nullptr;

			for (auto ArrayElem : l_playerRef)
			{
				if (ArrayElem->ActorHasTag("tag_shield"))
				{
					l_ShieldPlayerRef = ArrayElem;
				}
			}

			if (l_ShieldPlayerRef != nullptr)
			{
				// 有套盾对象（使用Enemy的SingleATK函数）
				DelayedTarget = l_ShieldPlayerRef;
				currentTarget = l_ShieldPlayerRef;
				SingleATK(l_ShieldPlayerRef);
				// 如果切换视角，切换至受击的玩家角色视角
				if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
				{
					return EAttackType::AT_NormalATK;
				}
				else
				{
					ACameraActor* TargetCA = BattleManager->RetrieveCamera("tag_c_boss");
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->
					SetViewTargetWithBlend(TargetCA);
					//UDaisyBlueprintFunctionLibrary::FindBattleManager()->SwitchAndHideOtherPlayerChars(true, l_ShieldPlayerRef);
					return EAttackType::AT_NormalATK;
				}
			}
			else
			{
				// 无套盾对象
				ABattlePlayer* l_TargetActor = nullptr;
				// 打乱数组l_playerRef后取第一个，相当于随机取一个
				l_TargetActor = l_playerRef[FMath::RandRange(0, (l_playerRef.Num()-1))];

				if(l_TargetActor == nullptr) return EAttackType::AT_EMAX;

				// 单体攻击
				DelayedTarget = l_TargetActor;
				currentTarget = l_TargetActor;
				SingleATK(l_TargetActor);

				// 如果切换视角，切换至受击的玩家角色视角
				if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
				{
					return EAttackType::AT_NormalATK;
				}
				else
				{
					ACameraActor* TargetCA = BattleManager->RetrieveCamera("tag_c_boss");
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->
					SetViewTargetWithBlend(TargetCA);
					//UDaisyBlueprintFunctionLibrary::FindBattleManager()->SwitchAndHideOtherPlayerChars(true, l_TargetActor);
					return EAttackType::AT_NormalATK;
				}
			}
		}
	}	
}

FString ABattleEnemy::RandomActionByRatio()
{
	// 按照概率选择攻击招式（在DataTable中设置概率，至少有一个攻击应为1.0概率，兜底）
	TArray<FString> l_StrChoices;
	choices.GenerateKeyArray(l_StrChoices);
	for (auto ArrayElem : l_StrChoices)
	{
		//必然存在，故无需用Contain检查；若为空，则报错
		if(!choices.Contains(ArrayElem)) return "None";

		bool l_selected = UKismetMathLibrary::RandomBoolWithWeight(*(choices.Find(ArrayElem)));
		if (l_selected)
		{
			// 立刻跳出
			return ArrayElem;
		}		
	}

	return "None";
}

void ABattleEnemy::UpdateLockIcon(bool bHide)
{
	LockIcon->bHiddenInGame = bHide;
}

void ABattleEnemy::RefreshActionValueBySpd()
{
	UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet);
	ActionValue = Distance / DaisyAS->GetSpeed();
}

void ABattleEnemy::HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
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

	HandleIndicatorNums(GetActorLocation(),ReceivedHPDmg,false,FColor::Red);

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

void ABattleEnemy::SetATK(EAttackType ATKType, int32 AttackCountInOneCycle)
{
	FBuffInfo tempBuffInfo;
	tempBuffInfo.BuffRatio = 0.0f;
	tempBuffInfo.BuffType = EBuffTypes::BT_EMAX;

	if (ATKType == EAttackType::AT_DelayATK_E)
	{
		SetDelayedTarget(true, currentTarget);
	}
	else
	{
		if (bRadialATK)
		{
			for (auto ArrayElem : currentTargetsArr)
			{
				ICombatInterface* tempInterface = Cast<ICombatInterface>(ArrayElem);
				if (tempInterface)
				{
					tempInterface->HitHandle(this, (totalATK / AttackCountInOneCycle), 0.0f, tempBuffInfo);
				}
			}
		}
		else
		{
			ICombatInterface* tempInterface = Cast<ICombatInterface>(currentTarget);
			if (tempInterface)
			{
				tempInterface->HitHandle(this, (totalATK / AttackCountInOneCycle), 0.0f, tempBuffInfo);
			}
		}
	}
}

void ABattleEnemy::Die()
{
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

void ABattleEnemy::AddWeakness(const TArray<FGameplayTag>& Weak)
{
	for (auto Tag : Weak)
	{
		AbilitySystemComponent->AddLooseGameplayTag(Tag);
	}
}

void ABattleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	FString s = DataRow.ToString();
	EnemyAtr = *(EnemyCharsDT->FindRow<FEnemyCharAttributes>(DataRow, s, true));
	
	InitAbilityActorInfo();
	InitializeDefaultAttributes();
	AddCharacterAbilities();
	AddWeakness(WeakNess);
	InitializeData();

	if (UDaisyUserWidget* DaisyUserWidget = Cast<UDaisyUserWidget>(HeadBar->GetUserWidgetObject()))
	{
		DaisyUserWidget->SetWidgetController(this);
	}
	
	if (const UDaisyAttributeSet* DaisyAS = Cast<UDaisyAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DaisyAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DaisyAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DaisyAS->GetToughnessAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnToughnessChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(DaisyAS->GetMaxToughnessAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxToughnessChanged.Broadcast(Data.NewValue);
			}
		);
		OnHealthChanged.Broadcast(DaisyAS->GetHealth());
		OnMaxHealthChanged.Broadcast(DaisyAS->GetMaxHealth());
		OnToughnessChanged.Broadcast(DaisyAS->GetToughness());
		OnMaxToughnessChanged.Broadcast(DaisyAS->GetMaxToughness());
	}
}
