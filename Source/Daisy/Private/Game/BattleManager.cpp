

#include "Game/BattleManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"
#include "Character/DaisyCharacter.h"
#include "Character/BattleEnemy.h"
#include "Character/DaisyEnemyCharacter.h"
#include "Character/BattlePlayer.h"
#include "Player/DaisyPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BattlePawn.h"
#include "Camera/CameraActor.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Debug/DebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Game/DaisyGameInstance.h"
#include "Interface/AnimInterface.h"
#include "UI/BattleUserWidget.h"
#include "UI/DaisyUserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystem/DaisyAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"


ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = true;
	FDaisyGameplayTags Tag = FDaisyGameplayTags::Get();
	/*
	TagColorMap.Add(Tag.Damage_Fire,FColor::Red);
	TagColorMap.Add(Tag.Damage_Lightning,FColor::Blue);
	TagColorMap.Add(Tag.Damage_Physical,FColor::Orange);
	TagColorMap.Add(Tag.Damage_Quantum,FColor::Purple);
	*/
}

void ABattleManager::InitBattle(ADaisyCharacter* Player, ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo)
{
	Player_World = Player;
	Enemy_World = Enemy;
	EnemyTeamInfo = EnemyInfo;
	PlayerTeamInfo = Cast<ADaisyPlayerController>(UGameplayStatics::GetPlayerController(this,0))->PlayerTeamInfo;

	//初始化
	A1_PreInitializeBattle();

	GetWorld()->GetTimerManager().SetTimer(DisplayEnemyTimerHandle,
		this,
		&ABattleManager::PostInitializeBattle,
		EnemyDisplayTime,
		false);
}

void ABattleManager::PostInitializeBattle()
{
	//计算行动值
	B1a_CalculateActionValue();
}

EBattleFlags ABattleManager::CheckGameOver(TMap<ABattleEnemy*, float> eArr, TMap<ABattlePlayer*, float> pArr)
{
	// 若敌人数组为0，则玩家胜利
	if(eArr.Num() == 0) return EBattleFlags::BF_PlayerWin;
	// 若玩家数组为0，则敌人胜利
	if(pArr.Num() == 0) return EBattleFlags::BF_EnemyWin;
	// 否则继续战斗循环
	return EBattleFlags::BF_ContinueBattle;
}

void ABattleManager::A1_PreInitializeBattle()
{
	ProgressPhase = EProgressPhase::PP_A1_PreInitialization;
	
	Enemies_Arr.Empty();
	Dead_Enemies_Arr.Empty();
	Player_Arr.Empty();
	Dead_Player_Arr.Empty();
	
	// 生成战斗时的指挥Pawn，确定是动态摄像机还是固定摄像机（BOSS、测试时使用），停止探索模式下的玩家角色
	ChangeCameraAndStopMovement();
	
	// 根据敌人阵容信息在指定站位生成敌人
	SpawnEnemiesAndDecideLocation();

	// 根据玩家队伍信息在指定站位生成玩家角色
	SpawnPlayersAndDecideLocation();

	//生成UI
	if (BattleLayoutClass)
	{
		BattleLayout = CreateWidget<UBattleUserWidget>(GetWorld(),BattleLayoutClass);
		if (BattleLayout)
		{
			BattleLayout->ConstructDeferred(this);
			BattleLayout->AddToViewport();
			BattleLayout->DisplaySkillPoints(SkillPoints);
		}
	}
}

void ABattleManager::A2_BattleEnd(EBattleFlags endResult)
{
	// 若玩家胜利，从战斗状态过渡回探索状态；如果失败，则退出游戏
	ProgressPhase = EProgressPhase::PP_A2_BattleEnd;

	BattleLayout->BattleOverHint();

	GetWorld()->GetTimerManager().SetTimer(HandleDelaysTimerHandle, this,
		&ABattleManager::HandleDelays, 1.5f, false);
}

void ABattleManager::HandleDelays()
{
	// 去除战斗UI
	BattleLayout->RemoveFromParent();
	BattleLayout = nullptr;

	// 1s后镜头开始变暗，切换回探索模式
	GetWorld()->GetTimerManager().SetTimer(BattleEndCameraStartingFadeTimerHandle, this,
		&ABattleManager::BattleEndCameraStartingFade, 1.0f, false);
}

void ABattleManager::BattleEndCameraStartingFade()
{
	// 开始变暗
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->
		StartCameraFade(0.0f, 1.0f, 1.0f, FColor::Black, false, true);

	// 再延迟1.1s后，回复屏幕显示
	GetWorld()->GetTimerManager().SetTimer(CleanBattleFieldTimerHandle, this,
		&ABattleManager::CleanBattleField, 1.1f, false);
}

void ABattleManager::CleanBattleField()
{
	ActivePlayerRef = nullptr;
	IndexForLockedTarget = 0;
	for (auto ArrayElem : Player_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Enemies_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Dead_Enemies_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Dead_Player_Arr)
	{
		ArrayElem->Destroy();
	}

	switch (curBattleFlag)
	{
	case EBattleFlags::BF_EMAX:
		break;
	case EBattleFlags::BF_ContinueBattle:
		break;
	case EBattleFlags::BF_PlayerWin:
		PlayerWin();
		break;
	case EBattleFlags::BF_EnemyWin:
		EnemyWin();
		break;
	default:
		break;
	}
}

void ABattleManager::PlayerWin()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Player_World);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Player_World);
	BattlePawn->Destroy();
	BattlePawn = nullptr;
	Enemy_World->Destroy();
	Player_World->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Player_World->bAttack = false;
	Player_World->FinishBattle();
	// 回复正常视野
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->
		StartCameraFade(1.0f, 0.0f, 0.5f, FColor::Black, false, false);
}

void ABattleManager::EnemyWin()
{
	// 延迟3s后退出游戏（也可以是主界面、取档界面等）
	GetWorld()->GetTimerManager().SetTimer(ExitGameTimerHandle, this,
		&ABattleManager::ExitGame, 1.1f, false);
}

void ABattleManager::ExitGame()
{
	// 退出游戏
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void ABattleManager::ExitBattle()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(Player_World);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Player_World);
	BattlePawn->Destroy();
	BattlePawn = nullptr;
	Player_World->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Player_World->FinishBattle();
	for (auto ArrayElem : Player_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Enemies_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Dead_Enemies_Arr)
	{
		ArrayElem->Destroy();
	}
	for (auto ArrayElem : Dead_Player_Arr)
	{
		ArrayElem->Destroy();
	}
	BattleLayout->RemoveFromParent();
	BattleLayout = nullptr;
	
	// 回复正常视野
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->
		StartCameraFade(1.0f, 0.0f, 0.5f, FColor::Black, false, false);
}

void ABattleManager::B1a_CalculateActionValue()
{
	ProgressPhase = EProgressPhase::PP_B1_CalculateActionValue;
	//检查玩家状态
	TMap<ABattleEnemy*,float> Enemy_ActionValue;
	TMap<ABattlePlayer*,float> Player_ActionValue;
	TMap<ACharacter*,float> CharacterQueue;
	TArray<ACharacter*> SortedCharacters;
	float WinnerActionValue = 0.0f;
	//检查玩家是否复活
	CheckPlayerRevive();
	//获取最新行动值，剔除不能行动的
	for (auto ArrayElem : Enemies_Arr)
	{
		if (!ArrayElem->bDead)
		{
			Enemy_ActionValue.Add(ArrayElem,ArrayElem->ActionValue);
			CharacterQueue.Add(ArrayElem,ArrayElem->ActionValue);
		}
		else
		{
			ArrayElem->OnEnemyTurnEnd.RemoveDynamic(this, &ABattleManager::EnemyTurnEnd);
			ArrayElem->OnEnemyDeath.RemoveDynamic(this, &ABattleManager::EnemyDeath);
			
			Dead_Enemies_Arr.Add(ArrayElem);
		}
	}
	for (auto ArrayElem : Player_Arr)
	{
		if (!ArrayElem->bDead)
		{
			Player_ActionValue.Add(ArrayElem,ArrayElem->ActionValue);
			CharacterQueue.Add(ArrayElem,ArrayElem->ActionValue);
		}
		else
		{
			Dead_Player_Arr.Add(ArrayElem);
		}
	}

	//刷新两个数组，排除死亡的
	Enemy_ActionValue.GenerateKeyArray(Enemies_Arr);
	Player_ActionValue.GenerateKeyArray(Player_Arr);

	//排序
	TMap<ACharacter*,float> NumDummy;
	NumDummy = CharacterQueue;
	for (auto ArrayElem : NumDummy)
	{
		TArray<float> LocalFloats;
		TArray<ACharacter*> LocalCharacters;
		int32 minIndex;
		float minValue;
		CharacterQueue.GenerateValueArray(LocalFloats);
		CharacterQueue.GenerateKeyArray(LocalCharacters);
		UKismetMathLibrary::MinOfFloatArray(LocalFloats,minIndex,minValue);
		SortedCharacters.Add(LocalCharacters[minIndex]);
		CharacterQueue.Remove(LocalCharacters[minIndex]);
	}

	//获取行动值最小的角色的行动值
	if (SortedCharacters[0] == nullptr) return;
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(SortedCharacters[0]);
	if (CombatInterface == nullptr) return;
	CombatInterface ->GetActionValue(WinnerActionValue);

	//更新其余角色的行动值
	for (auto ArrayElem : SortedCharacters)
	{
		ICombatInterface* CombatInterface_temp = Cast<ICombatInterface>(ArrayElem);
		if (CombatInterface == nullptr) return;
		CombatInterface_temp->UpdateActionValue(WinnerActionValue);
	}

	//更新UI的角色执行顺序
	BattleLayout->RefreshActionOrder(SortedCharacters);
	for (auto ArrayElem : Enemies_Arr)
	{
		//隐藏锁定标志
		ArrayElem->UpdateLockIcon(true);
	}

	//检查是否战斗结束
	curBattleFlag = CheckGameOver(Enemy_ActionValue, Player_ActionValue);
	switch (curBattleFlag)
	{
	case EBattleFlags::BF_EMAX:
		return;
		break;
	case EBattleFlags::BF_ContinueBattle:
		// 跳出，继续执行后续内容
			break;
	case EBattleFlags::BF_PlayerWin:
		A2_BattleEnd(curBattleFlag);
		return;
		break;
	case EBattleFlags::BF_EnemyWin:
		A2_BattleEnd(curBattleFlag);
		return;
		break;
	}
	// 当前行动对象进入准备状态（判断敌人和玩家的数组中行动值谁最小）
	TArray<float> l_LocalFloatsEAV;
	TArray<float> l_LocalFloatsPAV;
	int32 minIndex_E;
	float minValue_E;
	int32 minIndex_P;
	float minValue_P;
	Enemy_ActionValue.GenerateValueArray(l_LocalFloatsEAV);
	Player_ActionValue.GenerateValueArray(l_LocalFloatsPAV);
	UKismetMathLibrary::MinOfFloatArray(l_LocalFloatsEAV, minIndex_E, minValue_E);
	UKismetMathLibrary::MinOfFloatArray(l_LocalFloatsPAV, minIndex_P, minValue_P);
	if (minValue_E > minValue_P)
	{
		// 玩家角色的行动值小，玩家行动，进入B2a阶段
		TArray<ABattlePlayer*> l_LocalPlayerChars;
		Player_ActionValue.GenerateKeyArray(l_LocalPlayerChars);
		B2a_HandlePlayerAttack(l_LocalPlayerChars[minIndex_P]);
	}
	else
	{
		// 敌人的行动值小，敌人行动，进入B2b阶段
		TArray<ABattleEnemy*> l_LocalEnemyChars;
		Enemy_ActionValue.GenerateKeyArray(l_LocalEnemyChars);
		B2b_HandleEnemyAttack(l_LocalEnemyChars[minIndex_E]);
	}
}

void ABattleManager::B1b_CalculateActionValue()
{
	//使用在需要刷新行动值，但不进入下一回合时使用
	
	ProgressPhase = EProgressPhase::PP_B1_CalculateActionValue;
	//检查玩家状态
	TMap<ABattleEnemy*,float> Enemy_ActionValue;
	TMap<ABattlePlayer*,float> Player_ActionValue;
	TMap<ACharacter*,float> CharacterQueue;
	TArray<ACharacter*> SortedCharacters;
	float WinnerActionValue = 0.0f;
	
	//检查玩家是否复活
	CheckPlayerRevive();
	//获取最新行动值，剔除不能行动的
	for (auto ArrayElem : Enemies_Arr)
	{
		if (!ArrayElem->bDead)
		{
			Enemy_ActionValue.Add(ArrayElem,ArrayElem->ActionValue);
			CharacterQueue.Add(ArrayElem,ArrayElem->ActionValue);
		}
		else
		{
			Dead_Enemies_Arr.Add(ArrayElem);
		}
	}
	for (auto ArrayElem : Player_Arr)
	{
		if (!ArrayElem->bDead)
		{
			Player_ActionValue.Add(ArrayElem,ArrayElem->ActionValue);
			CharacterQueue.Add(ArrayElem,ArrayElem->ActionValue);
		}
		else
		{
			Dead_Player_Arr.Add(ArrayElem);
		}
	}

	//刷新两个数组，排除死亡的
	Enemy_ActionValue.GenerateKeyArray(Enemies_Arr);
	Player_ActionValue.GenerateKeyArray(Player_Arr);

	//排序
	TMap<ACharacter*,float> NumDummy;
	NumDummy = CharacterQueue;
	for (auto ArrayElem : NumDummy)
	{
		TArray<float> LocalFloats;
		TArray<ACharacter*> LocalCharacters;
		int32 minIndex;
		float minValue;
		CharacterQueue.GenerateValueArray(LocalFloats);
		CharacterQueue.GenerateKeyArray(LocalCharacters);
		UKismetMathLibrary::MinOfFloatArray(LocalFloats,minIndex,minValue);
		SortedCharacters.Add(LocalCharacters[minIndex]);
		CharacterQueue.Remove(LocalCharacters[minIndex]);
	}

	//获取行动值最小的角色的行动值
	if (SortedCharacters[0] == nullptr) return;
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(SortedCharacters[0]);
	if (CombatInterface == nullptr) return;
	CombatInterface ->GetActionValue(WinnerActionValue);

	//更新其余角色的行动值
	for (auto ArrayElem : SortedCharacters)
	{
		ICombatInterface* CombatInterface_temp = Cast<ICombatInterface>(ArrayElem);
		if (CombatInterface == nullptr) return;
		CombatInterface_temp->UpdateActionValue(WinnerActionValue);
	}

	//更新UI的角色执行顺序
	BattleLayout->RefreshActionOrder(SortedCharacters);
	for (auto ArrayElem : Enemies_Arr)
	{
		//隐藏锁定标志
		ArrayElem->UpdateLockIcon(true);
	}

	//检查是否战斗结束
	curBattleFlag = CheckGameOver(Enemy_ActionValue, Player_ActionValue);
	switch (curBattleFlag)
	{
	case EBattleFlags::BF_EMAX:
		return;
		break;
	case EBattleFlags::BF_ContinueBattle:
		// 跳出，继续执行后续内容
			break;
	case EBattleFlags::BF_PlayerWin:
		A2_BattleEnd(curBattleFlag);
		return;
		break;
	case EBattleFlags::BF_EnemyWin:
		A2_BattleEnd(curBattleFlag);
		return;
		break;
	}
}

void ABattleManager::CheckPlayerRevive()
{
	// 若复活，重新在行动列表上显示
	TArray<ABattlePlayer*> l_SortedRefs;
	// 为了不卡顿，分离deadPlayerRefArr遍历时的对象
	TArray<ABattlePlayer*> l_d_arr = Dead_Player_Arr;
	for (auto ArrayElem : l_d_arr)
	{
		if (!ArrayElem->bDead)
		{
			Player_Arr.AddUnique(ArrayElem);
			Dead_Player_Arr.Remove(ArrayElem);
		}
	}

	// 按队伍顺序，在玩家角色数组中重新排列位置
	TMap<ABattlePlayer*, int32> l_AbsoluteIndex;
	TMap<ABattlePlayer*, int32> l_FilteredIndex;
	TArray<ABattlePlayer*> l_pr_arr;
	TeamInstForUI.GenerateValueArray(l_pr_arr);
	for (int32 i = 0; i < l_pr_arr.Num(); i++)
	{
		l_AbsoluteIndex.Add(l_pr_arr[i], i+1);
	}
	for (ABattlePlayer* ArrayElem : Player_Arr)
	{
		
		int32 deadPlayerIndex = *(l_AbsoluteIndex.Find(ArrayElem));
		l_FilteredIndex.Add(ArrayElem, deadPlayerIndex);
	}
	for (int32 i = 0; i <= 3; i++)
	{
		TArray<int32> l_int_arr;
		int32 min_index;
		int32 min_val;
		l_FilteredIndex.GenerateValueArray(l_int_arr);
		UKismetMathLibrary::MinOfIntArray(l_int_arr, min_index, min_val);
		TArray<ABattlePlayer*> l_player_arr;
		l_FilteredIndex.GenerateKeyArray(l_player_arr);
		if(!l_player_arr.IsValidIndex(min_index)) return;
		ABattlePlayer* l_player = l_player_arr[min_index];
		if(l_player == nullptr) return;
		l_SortedRefs.AddUnique(l_player);
		l_FilteredIndex.Remove(l_player);
	}

	Player_Arr = l_SortedRefs;
}

void ABattleManager::StartFollowingATK(AActor* atkTarget)
{
	ProgressPhase = EProgressPhase::PP_B2c_Animating;
	HideAllLockedIcons();
	if(ActivePlayerRef == nullptr) return;
	// 追加攻击只打一人（示例）
	ActivePlayerRef->SingleAtk(atkTarget, false, IsMeleeAction(), EAttackType::AT_FollowTK);

	FString name = ActivePlayerRef->playerAtr.CharName;
	BattleLayout->FollowATKHint(name);
}

void ABattleManager::B2a_HandlePlayerAttack(ABattlePlayer* activePlayerChar)
{
	ActivePlayerRef = activePlayerChar;

	ProgressPhase = EProgressPhase::PP_B2a_PlayerActionTime;

	//设置初始锁定
	DisplayLockedIconsAndSetTargets();

	//更新UI按钮
	if (!ActivePlayerRef) return;
	BattleLayout->SwitchATKMode(ActivePlayerRef->AttackType);
	BattleLayout->SwitchUIImage(ActivePlayerRef->Image);
	//镜头切换
	if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
	{
		// None
	}
	else
	{
		//隐藏其他玩家
		SwitchAndHideOtherPlayerChars(true,ActivePlayerRef);
			
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->
		SetViewTargetWithBlend(ActivePlayerRef,1.f);
	}
}

void ABattleManager::B2b_HandleEnemyAttack(ABattleEnemy* activeEnemyChar)
{
	
	ProgressPhase = EProgressPhase::PP_B2b_EnemyActionTime;
	BattleLayout->HideATKButtons();
	if (activeEnemyChar->bStun)
	{
		activeEnemyChar->RecoverFromStun();
		B3_TurnEnd(activeEnemyChar,true);
	}
	else
	{
		// 攻击逻辑
		EAttackType EnemyAtkType = activeEnemyChar->ActionDecision(Player_Arr);	
		//
		BattleLayout->HandlePhaseHintEnemy(activeEnemyChar,UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight,EnemyAtkType);
		
	}
}

void ABattleManager::B3_TurnEnd(AActor* EndTurnActor, bool bConsumeTurn)
{
	Damage_End = 0;
	UpdateDamage(false);
	EnemyAttackTarget = nullptr;
	EnemyAttackTargets.Empty();

	UAbilitySystemComponent* SourASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EndTurnActor);
	ABattlePlayer* Player = Cast<ABattlePlayer>(EndTurnActor);
	if(Player)
	{
		if (!Player->BuffMap.IsEmpty())
		{
			for (auto& Pair : Player->BuffMap)
			{
				if (Pair.Value.BuffNums == 0)
				{
					SourASC->RemoveActiveGameplayEffect(Pair.Key);
					Player->BuffMap.Remove(Pair.Key);
				}
				else
				{
					Pair.Value.BuffNums -= 1;
				}
			}
			Player->GetCameraBoom()->SetRelativeRotation(Player->StartRotation);
		}
	}
	
	//回合结束
	ProgressPhase = EProgressPhase::PP_B3_TurnEnd;
	//如果释放大招，删除UI
	RemoveUltimateTurn(EndTurnActor);
	
	//镜头切换
	if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->
		SetViewTargetWithBlend(RetrieveCamera(FName(*fixedCA)));
	}
	//消耗回合数的情况下，减少Buff的持续回合数，重置ActionValue，检查追加攻击。
	if (bConsumeTurn)
	{
		// 减少Buff的持续回合数（如套盾）
		ICombatInterface* tempCombatInterface = Cast<ICombatInterface>(EndTurnActor);
		if (tempCombatInterface != nullptr)
		{
			tempCombatInterface->CountBuffsTimer();
		}
	}
	ResetActionValueAndATKType(bConsumeTurn,EndTurnActor);
	//重置UI面板
	//ABattlePlayer* Player = Cast<ABattlePlayer>(EndTurnActor);
	if (Player)
	{
		BattleLayout->HandleStatsPanelAnimating(Player,false);
	}
	//检测是否执行追加攻击
	if (bTryFollowingATK && GuardedChar != nullptr)
	{
		// 发起追加攻击
		bTryFollowingATK = false;
		B1b_CalculateActionValue();
		// 检查守护者是否死亡、晕眩
		ICombatInterface* tempCombatInterface = Cast<ICombatInterface>(GuardedChar);
		if (tempCombatInterface != nullptr)
		{
			tempCombatInterface->TryFollowingATK();
		}
		GuardedChar = nullptr;
		// 跳出
		return;
	}
	//检查是否有大招在等待
	if (UltimatePlayerQueue.Num() > 0)
	{
		//释放大招
		//如果敌人回合按下大招，回合结束检查玩家是否存活
		for (auto ArrayElem : UltimatePlayerQueue)
		{
			if (ArrayElem -> bDead)
			{
				UltimatePlayerQueue.Remove(ArrayElem);
			}
		}
		BattleLayout->RefreshUItimateOrder(UltimatePlayerQueue);
		
		if (UltimatePlayerQueue[0] != nullptr)
		{
			//刷新行动值B1b后执行大招准备
			B1b_CalculateActionValue();
			ReadyForUltimate(UltimatePlayerQueue[0]);
			return;
		}
		else
		{
			B1a_CalculateActionValue();
			return;
		}
	}
	else
	{
		B1a_CalculateActionValue();
	}
}

void ABattleManager::ResetActionValueAndATKType(bool bConsumeTurn, AActor* TempEndActor)
{
	if (bConsumeTurn)
	{
		ICombatInterface* Interface = Cast<ICombatInterface>(TempEndActor);
		if (Interface)
		{
			Interface->RefreshActionValueBySpd();
		}
	}
	//不消耗回合就不刷新行动值
	//reset ATKType
	if (ActivePlayerRef)
	{
		ActivePlayerRef->AttackType = EAttackType::AT_NormalATK;
	}
	ActivePlayerRef = nullptr;
}

void ABattleManager::SwitchEnemyLockIcon(bool bNext)
{
	if (ProgressPhase != EProgressPhase::PP_B2a_PlayerActionTime) return;

	if (NotResurrectSkill())
	{
		CalculateLockIndex(bNext);
	}
	else
	{
		if (IndexForLockedTarget == -1)
		{
			
		}
		else
		{
			CalculateLockIndex(bNext);
		}
	}
}

bool ABattleManager::IsBuffTarget()
{
	if (!ActivePlayerRef) return false;
	bool result = ActivePlayerRef->playerAtr.BuffSkillStats.Contains(ActivePlayerRef->AttackType);
	return result;
}

void ABattleManager::SetMultipeEnemyLocks()
{
	if (IsMultipleTargets())
	{
		CurrentEnemyTargets.Empty();
		if (Enemies_Arr.IsValidIndex(IndexForLockedTarget))
		{
			CurrentEnemyTargets.Add(Enemies_Arr[IndexForLockedTarget]);
		}
		if (Enemies_Arr.IsValidIndex(IndexForLockedTarget-1))
		{
			CurrentEnemyTargets.Add(Enemies_Arr[IndexForLockedTarget-1]);
		}
		if (Enemies_Arr.IsValidIndex(IndexForLockedTarget+1))
		{
			CurrentEnemyTargets.Add(Enemies_Arr[IndexForLockedTarget+1]);
		}
		switch (Enemies_Arr[IndexForLockedTarget]->PositionID)
		{
		case 0:
			break;
		case 1:
			TargetSpringArmYaw  = -60.0f;
			break;
		case 2:
			TargetSpringArmYaw  = -50.0f;
			break;
		case 3:
			TargetSpringArmYaw  = -40.0f;
			break;
		case 4:
			TargetSpringArmYaw  = -30.0f;
			break;
		case 5:
			TargetSpringArmYaw  = -20.0f;
			break;
		case 6:
			TargetSpringArmYaw  = -10.0f;
			break;
		}
		ShowEnemyMultipleLockIcons(CurrentEnemyTargets);
	}
	else
	{
		CurrentEnemyTarget = Enemies_Arr[IndexForLockedTarget];
		LastClickedActor = CurrentEnemyTarget;
		switch (CurrentEnemyTarget->PositionID)
		{
		case 0:
			break;
		case 1:
			TargetSpringArmYaw  = -60.0f;
			break;
		case 2:
			TargetSpringArmYaw  = -50.0f;
			break;
		case 3:
			TargetSpringArmYaw  = -40.0f;
			break;
		case 4:
			TargetSpringArmYaw  = -30.0f;
			break;
		case 5:
			TargetSpringArmYaw  = -20.0f;
			break;
		case 6:
			TargetSpringArmYaw  = -10.0f;
			break;
		}
		ShowEnemyLockIconByIndex(IndexForLockedTarget);
	}
}

bool ABattleManager::IsMultipleTargets()
{
	if (!ActivePlayerRef) return false;
	if (ActivePlayerRef->playerAtr.MultipleTargets.Contains(ActivePlayerRef->AttackType))
	{
		bool result = *(ActivePlayerRef->playerAtr.MultipleTargets.Find(ActivePlayerRef->AttackType));
		return result;
	}
	else
	{
		return false;
	}
}

void ABattleManager::ShowEnemyLockIconByIndex(int32 index)
{
	for (auto ArrayElement:Enemies_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
	if (Enemies_Arr[index])
	{
		Enemies_Arr[index]->UpdateLockIcon(false);
	}
	//TODO:
}

void ABattleManager::ShowEnemyMultipleLockIcons(TArray<ABattleEnemy*> indexRefs)
{
	for (auto Element : Enemies_Arr)
	{
		Element->UpdateLockIcon(true);
	}
	for (auto ArrayElement:indexRefs)
	{
		if (!ArrayElement) return;
		if (ArrayElement->bDead) return;
		ArrayElement->UpdateLockIcon(false);
	}
}

void ABattleManager::ShowPlayerLockIconByIndex(int32 index)
{
	for (auto ArrayElement:Player_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
	if (Player_Arr[index])
	{
		Player_Arr[index]->UpdateLockIcon(false);
	}
}

void ABattleManager::ShowPlayerMultipleLockIcons(TArray<ABattlePlayer*> indexRefs)
{
	for (auto ArrayElement:Player_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}

	for (auto ArrayElement:indexRefs)
	{
		if (ArrayElement)
		{
			ArrayElement->UpdateLockIcon(false);
		}
	}
}

bool ABattleManager::NotResurrectSkill()
{
	bool result = false;
	if (!ActivePlayerRef) return false;
	if (ActivePlayerRef->playerAtr.BuffSkillStats.Contains(ActivePlayerRef->AttackType))
	{
		FBuffInfo tempFuffType = *(ActivePlayerRef->playerAtr.BuffSkillStats.Find(ActivePlayerRef->AttackType));
		result = tempFuffType.BuffType != EBuffTypes::BT_Resurrection;
	}
	else
	{
		result = true;
	}
	return result;
}

void ABattleManager::CalculateLockIndex(bool bNext)
{
	int32 t1 = -1;
	//判断是否为增益技能
	if (IsBuffTarget())
	{
		if (NotResurrectSkill())
		{
			t1 = Player_Arr.Num();
		}
		else
		{
			t1 = Dead_Player_Arr.Num();
		}
	}
	else
	{
		t1 = Enemies_Arr.Num();
	}
	//切换锁定目标
	if (bNext)
	{
		if ((IndexForLockedTarget +1) < t1)
		{
			IndexForLockedTarget += 1;
		}
		else
		{
			IndexForLockedTarget = 0;
		}
	}
	else
	{ 
		if (IndexForLockedTarget > 0)
		{
			IndexForLockedTarget -= 1;
		}
		else
		{
			IndexForLockedTarget = t1 - 1;
		}
	}

	//检查是否是复活
	if (NotResurrectSkill())
	{
		if (IsBuffTarget())
		{
			SetPlayerLockedIcons();
		}
		else
		{
			SetMultipeEnemyLocks();
		}
	}
	else
	{
		SetDeadPlayerLockedIcon();
	}
}

void ABattleManager::SetPlayerLockedIcons()
{
	if (IsMultipleTargets())
	{
		CurrentPlayerTargets = Player_Arr;
		//向队伍全体施加
		ShowPlayerMultipleLockIcons(CurrentPlayerTargets);
	}
	else
	{
		if (!Player_Arr.IsValidIndex(IndexForLockedTarget)) return;
		CurrentPlayerTarget = Player_Arr[IndexForLockedTarget];
		LastClickedActor = CurrentPlayerTarget;
		ShowPlayerLockIconByIndex(IndexForLockedTarget);
	}
}

void ABattleManager::HideAllLockedIcons()
{
	for (auto ArrayElement:Player_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
	for (auto ArrayElement:Enemies_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
	for (auto ArrayElement:Dead_Player_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
	for (auto ArrayElement:Dead_Enemies_Arr)
	{
		ArrayElement->UpdateLockIcon(true);
	}
}

void ABattleManager::DisplayLockedIconsAndSetTargets()
{
	HideAllLockedIcons();

	BattleLayout->HandleStatsPanelAnimating(ActivePlayerRef,true);
	
	//是否切换到buff魔法
	if (IsBuffTarget())
	{
		UpdatePlayerLockedIconToMultiple();
	}
	else
	{
		UpdateEnemyLockedIconToMultiple();
	}
}

void ABattleManager::UpdatePlayerLockedIconToMultiple()
{
	if (NotResurrectSkill())
	{
		IndexForLockedTarget = (Player_Arr.Num()-1) / 2;
		SetPlayerLockedIcons();
	}
	else
	{
		if (Dead_Player_Arr.Num() != 0)
		{
			IndexForLockedTarget = 0;
		}
		else
		{
			IndexForLockedTarget = -1;
		}
		if (!Dead_Player_Arr.IsValidIndex(IndexForLockedTarget)) return;
		if (Dead_Player_Arr[IndexForLockedTarget])
		{
			CurrentPlayerTarget = Dead_Player_Arr[IndexForLockedTarget];
			Dead_Player_Arr[IndexForLockedTarget]->UpdateLockIcon(false);
		}
	}
}

void ABattleManager::UpdateEnemyLockedIconToMultiple()
{
	//筛选敌人
	TArray<ABattleEnemy*> ValidEnemies;
	
	for (auto ArrayElement:Enemies_Arr)
	{
		if (!ArrayElement) return;
		if (!ArrayElement->bDead)
		{
			ValidEnemies.Add(ArrayElement);
		}
	}
	IndexForLockedTarget = (ValidEnemies.Num()-1) / 2;
	if (!Enemies_Arr.IsValidIndex(IndexForLockedTarget)) return;
	SetMultipeEnemyLocks();
}

bool ABattleManager::IsMeleeAction()
{
	if (!ActivePlayerRef) return true;
	if (ActivePlayerRef->playerAtr.MeleeAction.Contains(ActivePlayerRef->AttackType))
	{
		bool result = *(ActivePlayerRef->playerAtr.MeleeAction.Find(ActivePlayerRef->AttackType));
		return result;
	}
	else
	{
		return true;
	}
}

void ABattleManager::SwitchAndHideOtherPlayerChars(bool bHideOther, ABattlePlayer* ActiveChar)
{
	TArray<ABattlePlayer*> TempPlayers;
	TeamInstForUI.GenerateValueArray(TempPlayers);
	for (auto ArrayElem : TempPlayers)
	{
		//ArrayElem->SetHiddenForPlayer(bHideOther);
	}
	ActiveChar->SetHiddenForPlayer(false);
	// 自动切换视角
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTargetWithBlend(ActiveChar);
}

void ABattleManager::ExecuteAction(EAttackType ATKType)
{
	if (!ActivePlayerRef) return;
	switch (ATKType)
	{
	case EAttackType::AT_EMAX:
		break;
	case EAttackType::AT_NormalATK:
		if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime &&
			ActivePlayerRef->AttackType != EAttackType::AT_Ultimate)
		{
			HandlePlayerATK(EAttackType::AT_NormalATK);
		}
		break;
	case EAttackType::AT_SkillATK:
		if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime &&
			ActivePlayerRef->AttackType != EAttackType::AT_Ultimate)
		{
			HandlePlayerATK(EAttackType::AT_SkillATK);
		}
		break;
	case EAttackType::AT_FollowTK:
		break;
	case EAttackType::AT_Ultimate:
		ExecuteUltimate();
		break;
	case EAttackType::AT_DelayATK_E:
		break;
	}
}

void ABattleManager::HandlePlayerATK(EAttackType AttackType)
{
	if (!ActivePlayerRef) return;
	if (AttackType == EAttackType::AT_SkillATK)
	{
		if (SkillPoints <= 0)
		{
			UGameplayStatics::SpawnSound2D(GetWorld(),UnableSFX);
			return;
		}
	}
	if (ActivePlayerRef->AttackType != AttackType)
	{
		//改变姿态
		ActivePlayerRef->AttackType = AttackType;
		//重置锁定目标
		DisplayLockedIconsAndSetTargets();
		//是否切换镜头
		CameraForBuffSelections();
		//更改UI
		BattleLayout->SwitchATKMode(AttackType);
	}
	else
	{
		if (AttackType == EAttackType::AT_Ultimate)
		{
			if (IAnimInterface* TempInterface = Cast<IAnimInterface>(ActivePlayerRef->GetMesh()->GetAnimInstance()))
			{
				TempInterface->SetUltimateReadyVFX(false);
			}
		}

		//判断该动作是否影响多个对象
		AActor* TempTargetActor;
		bool bConsumeTurn = (AttackType == EAttackType::AT_NormalATK || AttackType == EAttackType::AT_SkillATK);
		if (IsMultipleTargets())
		{
			if (IsBuffTarget())
			{
				if (!CurrentPlayerTargets.IsValidIndex(0)) return;
				if (!CurrentPlayerTargets[0]) return;
				TArray<AActor*> TempTargets(CurrentPlayerTargets);
				HideAllLockedIcons();
				ProgressPhase = EProgressPhase::PP_B2c_Animating;
				ActivePlayerRef->MultipleAtk(TempTargets,bConsumeTurn,IsMeleeAction(),AttackType);
			}
			else
			{
				if (!CurrentEnemyTargets.IsValidIndex(0)) return;
				if (!CurrentEnemyTargets[0]) return;
				TArray<AActor*> TempTargets(CurrentEnemyTargets);
				HideAllLockedIcons();
				ProgressPhase = EProgressPhase::PP_B2c_Animating;
				ActivePlayerRef->MultipleAtk(TempTargets,bConsumeTurn,IsMeleeAction(),AttackType);
			}
		}
		else
		{
			if (IsBuffTarget())
			{
				if (!CurrentPlayerTarget) return;
				TempTargetActor = CurrentPlayerTarget;
			}
			else
			{
				if (!CurrentEnemyTarget) return;
				TempTargetActor = CurrentEnemyTarget;
			}
			HideAllLockedIcons();
			ProgressPhase = EProgressPhase::PP_B2c_Animating;
			ActivePlayerRef->SingleAtk(TempTargetActor,bConsumeTurn,IsMeleeAction(),AttackType);
		}

		//根据攻击类型，增加或消耗技能点
		int32 SkillPointsDelta = 0;
		switch (AttackType)
		{
		case EAttackType::AT_EMAX:
			break;
		case EAttackType::AT_NormalATK:
			SkillPointsDelta = 1;
			break;
		case EAttackType::AT_SkillATK:
			SkillPointsDelta = -1;
			break;
		case EAttackType::AT_FollowTK:
			break;
		case EAttackType::AT_Ultimate:
			break;
		case EAttackType::AT_DelayATK_E:
			break;
		}
		int32 skillPoints = SkillPoints + SkillPointsDelta;
		SkillPoints = UKismetMathLibrary::Clamp(skillPoints,0,5);

		BattleLayout->DisplaySkillPoints(skillPoints);
		//UI文字提示
		BattleLayout->HandlePhaseHint_CPP(ActivePlayerRef,AttackType);
		//战斗中隐藏UI
		BattleLayout->HideATKButtons();
		
	}
}

void ABattleManager::ExecuteUltimate()
{
	//检查大招Queue是否为空 ， 是否是玩家回合
	if (ProgressPhase == EProgressPhase::PP_B2a_PlayerActionTime && UltimatePlayerQueue.Num() > 0)
	{
		HandlePlayerATK(EAttackType::AT_Ultimate);
	}
}

void ABattleManager::EnterUltimate(int32 PlayerIndex)
{
	if (!TeamInstForUI.Contains(PlayerIndex)) return;
	ABattlePlayer* SpecifiedPlayer = *(TeamInstForUI.Find(PlayerIndex));
	if (SpecifiedPlayer == nullptr) return;
	//检查角色是否死亡
	bool bAlive = !SpecifiedPlayer->bDead;
	bool bP1 = ProgressPhase == EProgressPhase::PP_A1_PreInitialization;
	bool bP2 = ProgressPhase == EProgressPhase::PP_A2_BattleEnd;
	bool bP3 = ProgressPhase == EProgressPhase::PP_EMAX;
	if (!bAlive || bP1 || bP2 || bP3) return;
	
	//判断并消耗能量
	/*
	bool bFullEP = (SpecifiedPlayer->CurEnergy / SpecifiedPlayer->MaxEnergy) > 1.0f;
	if (!bFullEP) return;
	float AllEP = SpecifiedPlayer->MaxEnergy * (-1.0f);
	SpecifiedPlayer->HandleEP(EAttackType::AT_EMAX,true,AllEP);
	*/
	if (!SpecifiedPlayer->bCanUltimate) return;
	ClearEnergy(SpecifiedPlayer);
	SpecifiedPlayer->bCanUltimate = false;
	
	//加入大招序列
	UltimatePlayerQueue.Add(SpecifiedPlayer);
	BattleLayout->RefreshUItimateOrder(UltimatePlayerQueue);
	//判断是否是玩家回合
	if (ProgressPhase != EProgressPhase::PP_B2a_PlayerActionTime) return;
	if (ActivePlayerRef != UltimatePlayerQueue[0] && ActivePlayerRef != nullptr)
	{
		BattleLayout->HandleStatsPanelAnimating(ActivePlayerRef,false);
	}
	ReadyForUltimate(UltimatePlayerQueue[0]); 
}

void ABattleManager::ReadyForUltimate(ABattlePlayer* ReadyPlayer)
{
	ActivePlayerRef = ReadyPlayer;
	ActivePlayerRef->AttackType = EAttackType::AT_Ultimate;
	ProgressPhase = EProgressPhase::PP_B2a_PlayerActionTime;
	DisplayLockedIconsAndSetTargets();
	CameraForBuffSelections();
	IAnimInterface* TempInterface = Cast<IAnimInterface>(ActivePlayerRef->GetMesh()->GetAnimInstance());
	if (TempInterface)
	{
		TempInterface->SetUltimateReadyVFX(true);
	}
	BattleLayout->SwitchATKMode(EAttackType::AT_Ultimate);
}

void ABattleManager::RemoveUltimateTurn(AActor* CharRef)
{
	ABattlePlayer* Player = Cast<ABattlePlayer>(CharRef);
	if (Player == nullptr) return;
	bool bInUltimateTurn = Player->AttackType == EAttackType::AT_Ultimate;
	if (!bInUltimateTurn) return;
	UltimatePlayerQueue.RemoveAt(0);
	//UI
	BattleLayout->RefreshUItimateOrder(UltimatePlayerQueue);
}

void ABattleManager::CameraForBuffSelections()
{
	if (IsBuffTarget())
	{
		//增益
		SwitchAndHideOtherPlayerChars(false,ActivePlayerRef);
		
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(RetrieveCamera(FName(*buffCA)));
	}
	else
	{
		if (UDaisyBlueprintFunctionLibrary::GetGameInstance()->bBOSSFight)
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(RetrieveCamera(FName(*fixedCA)));
		}
		else
		{
			//隐藏其他玩家
			SwitchAndHideOtherPlayerChars(true,ActivePlayerRef);
			
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->
			SetViewTargetWithBlend(ActivePlayerRef,1.f);
		}
	}
}

void ABattleManager::EnemyDeath(ABattleEnemy* enemyRef, AActor* causerRef)
{
	ABattlePlayer* tempPlayerRef = Cast<ABattlePlayer>(causerRef);
	if (tempPlayerRef != nullptr)
	{
		tempPlayerRef->HandleEP(EAttackType::AT_EMAX, true, 10.0f);
	}	
}

void ABattleManager::EnemyTurnEnd(ABattleEnemy* enemyRef)
{
	// 敌人回合结束，消耗回合
	B3_TurnEnd(enemyRef, true);
}

void ABattleManager::SetDeadPlayerLockedIcon()
{
	if (! Dead_Player_Arr.IsValidIndex(IndexForLockedTarget)) return;
	
	HideAllLockedIcons();

	CurrentPlayerTarget = Dead_Player_Arr[IndexForLockedTarget];
	LastClickedActor = CurrentPlayerTarget;
	if (CurrentPlayerTarget)
	{
		CurrentPlayerTarget->UpdateLockIcon(false);
	}
}

void ABattleManager::ChangeCameraAndStopMovement()
{
	BattlePawn = Cast<ABattlePawn>(GetWorld()->SpawnActor<AActor>(BattlePawnClass,FVector(0,0,0),FRotator(0,0,0)));
	GetWorld()->GetFirstPlayerController()->Possess(BattlePawn);

	Player_World->GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 改变摄像机视角
	ACameraActor* TargetCA;
	UDaisyGameInstance* tempGI;
	tempGI = UDaisyBlueprintFunctionLibrary::GetGameInstance();
	if(tempGI == nullptr) return;

	if (tempGI->bBOSSFight)
	{
		TargetCA = RetrieveCamera(FName(*fixedCA));
	}
	else
	{
		TargetCA = RetrieveCamera(FName(*normalCA));
	}
	
	if(TargetCA == nullptr) return;
	ADaisyPlayerController* PC = 
		Cast<ADaisyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(PC == nullptr) return;
	PC->SetViewTargetWithBlend(TargetCA);
}

ACameraActor* ABattleManager::RetrieveCamera(FName Tag)
{
	for (auto ArrayElem : Cameras_Arr)
	{
		if (ArrayElem->ActorHasTag(Tag))
		{
			return ArrayElem;
		}
	}

	return nullptr;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(),ACameraActor::StaticClass(),TempActors);
	for (auto ArrayElem : TempActors)
	{
		Cameras_Arr.Add(Cast<ACameraActor>(ArrayElem));
	}

	// 获取敌人的位置
	TempActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), TempActors);
	for (auto ArrayElem : TempActors)
	{
		if (ArrayElem->ActorHasTag("enemyP"))
		{
			EnemySpawnPoints_Arr.Add(ArrayElem);
		}		
	}

	// 获取玩家角色的位置
	TempActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GEngine->GameViewport->GetWorld(), ATargetPoint::StaticClass(), TempActors);
	for (auto ArrayElem : TempActors)
	{
		if (ArrayElem->ActorHasTag("playerP"))
		{
			PlayerSpawnPoints_Arr.Add(ArrayElem);
		}
	}
	//平滑移动Buff镜头
	BuffCamera = RetrieveCamera(FName(*buffCA));
	BuffCameraOriginLocation = BuffCamera->GetActorLocation();
}

void ABattleManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ActivePlayerRef == nullptr) return;
	if (BuffCamera == nullptr) return;
	
	float Location_Y;
	
	if (ActivePlayerRef && ActivePlayerRef->GetCameraBoom())
	{
		CurrentSpringArmYaw = FMath::FInterpTo(CurrentSpringArmYaw, TargetSpringArmYaw, DeltaSeconds, 2);
		ActivePlayerRef->GetCameraBoom()->SetRelativeRotation(FRotator(0.0f, CurrentSpringArmYaw, 0.0f));
	}
	
	if (!IsBuffTarget()) return;
	if (IsMultipleTargets())
	{
		Location_Y = BuffCameraOriginLocation.Y;
	}
	else
	{
		AActor* TargetActor;
		if (CurrentPlayerTarget != nullptr)
		{
			TargetActor = CurrentPlayerTarget;
		}
		else
		{
			bool b1 = ActivePlayerRef->AttackType == EAttackType::AT_SkillATK;
			//FBuffInfo FBI = *(ActivePlayerRef->playerAtr.BuffSkillStats.Find(EAttackType::AT_SkillATK));
			//bool b2 = FBI.BuffType == EBuffTypes::BT_Resurrection;
			if (b1)
			{
				TargetActor = ActivePlayerRef;
			}
			else
			{
				return;
			}
		}
		Location_Y = TargetActor->GetActorLocation().Y;
	}

	float FinalY = FMath::FInterpTo(BuffCamera->GetActorLocation().Y, Location_Y, DeltaSeconds, 1.f);
	FVector FinalTargetLocation = FVector(BuffCameraOriginLocation.X,FinalY,BuffCameraOriginLocation.Z);
	BuffCamera->SetActorLocation(FinalTargetLocation);
}

FColor ABattleManager::GetColorByTag(const FGameplayTag& Tag) const
{
	// 使用Find方法检查映射中是否存在该标签
	if (const FColor* ColorPtr = TagColorMap.Find(Tag))
	{
		return *ColorPtr; // 返回找到的颜色
	}
	// 未找到标签时的处理
	UE_LOG(LogTemp, Warning, TEXT("Tag %s not found in TagColorMap!"), *Tag.ToString());
	// 返回默认颜色
	return FColor::Red;
}

FGameplayTag ABattleManager::GetTagByName(const FString& Name) const
{
	// 使用Find方法检查映射中是否存在该标签
	if (const FGameplayTag* Tag = StringTagMap.Find(Name))
	{
		return *Tag; // 返回找到的
	}
	// 未找到标签时的处理
	UE_LOG(LogTemp, Warning, TEXT("Tag %s not found in StringTagMap!"), *Name);
	// 返回默认
	return FGameplayTag();
}

FGameplayTag ABattleManager::GetTagByName_Enemy(const FString& Name) const
{
	// 使用Find方法检查映射中是否存在该标签
	if (const FGameplayTag* Tag = Enemy_StringTagMap.Find(Name))
	{
		return *Tag; // 返回找到的
	}
	// 未找到标签时的处理
	UE_LOG(LogTemp, Warning, TEXT("Tag %s not found in Enemy_StringTagMap!"), *Name);
	// 返回默认
	return FGameplayTag();
}

void ABattleManager::UpdateDamage(bool bShow) const
{
	OnDamageChanged.Broadcast(Damage_End, bShow);
}

void ABattleManager::ApplyEffect()
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActivePlayerRef);
	if(TargetASC == nullptr) return;

	check(InstantGameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(InstantGameplayEffectClass,1,EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

}

void ABattleManager::ClearEnergy(AActor* ClearTarget)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ClearTarget);
	if(TargetASC == nullptr) return;

	check(ClearEnergyGameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(ClearEnergyGameplayEffectClass,1,EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}
bool ABattleManager::IsTarget(AActor* ClickedTarget)
{
	if (IsBuffTarget())
	{
		ABattlePlayer* PlayerRef = Cast<ABattlePlayer>(ClickedTarget);
		if (PlayerRef == nullptr) return false;
	}
	else
	{
		ABattleEnemy* EnemyRef = Cast<ABattleEnemy>(ClickedTarget);
		if (EnemyRef == nullptr) return false;
	}
	return true;
}

void ABattleManager::ClickOnCharacter(AActor* ClickedTarget)
{
	if (ClickedTarget == nullptr) return;
	if (ProgressPhase != EProgressPhase::PP_B2a_PlayerActionTime) return;
	if (!IsTarget(ClickedTarget)) return;
	
	if (ClickedTarget != LastClickedActor)
	{
		LastClickedActor = ClickedTarget;
		if (IsBuffTarget())
		{
			ABattlePlayer* PlayerRef = Cast<ABattlePlayer>(LastClickedActor);
			if (PlayerRef == nullptr) return;
		}
		else
		{
			ABattleEnemy* EnemyRef = Cast<ABattleEnemy>(LastClickedActor);
			if (EnemyRef == nullptr) return;
		}
		if (NotResurrectSkill())
		{
			RetrieveIndex(LastClickedActor);
		}
		else
		{
			if (IndexForLockedTarget == -1) return;
			else
			{
				RetrieveIndex(LastClickedActor);
			}
		}
		if (NotResurrectSkill())
		{
			if (IsBuffTarget())
			{
				SetPlayerLockedIcons();
			}
			else
			{
				SetMultipeEnemyLocks();
			}
		}
	}
	else
	{
		if (ActivePlayerRef != nullptr)
		{
			ExecuteAction(ActivePlayerRef->AttackType);
		}
		return;
	}
}

void ABattleManager::RetrieveIndex(AActor* ClickedActorForIndex)
{
	if (Player_Arr.IsValidIndex(0))
	{
		for (int32 i = 0;i<Player_Arr.Num();i++)
		{
			if (ClickedActorForIndex == Player_Arr[i])
			{
				IndexForLockedTarget = i;
				return;
			}
		}
	}
	if (Enemies_Arr.IsValidIndex(0))
	{
		for (int32 i = 0;i<Enemies_Arr.Num();i++)
		{
			if (ClickedActorForIndex == Enemies_Arr[i])
			{
				IndexForLockedTarget = i;
				return;
			}
		}
	}
	if (Dead_Player_Arr.IsValidIndex(0))
	{
		for (int32 i = 0;i<Dead_Player_Arr.Num();i++)
		{
			if (ClickedActorForIndex == Dead_Player_Arr[i])
			{
				IndexForLockedTarget = i;
				return;
			}
		}
	}
	if (Dead_Enemies_Arr.IsValidIndex(0))
	{
		for (int32 i = 0;i<Dead_Enemies_Arr.Num();i++)
		{
			if (ClickedActorForIndex == Dead_Enemies_Arr[i])
			{
				IndexForLockedTarget = i;
				return;
			}
		}
	}
	IndexForLockedTarget = -1;
}

void ABattleManager::RetrieveEnemyPosition(int32 PosIndex, FVector& TargetPos, float& Yaw)
{
	for (auto ArrayElem : EnemySpawnPoints_Arr)
	{
		FString targetTag;
		targetTag = "e" + FString::FromInt(PosIndex);
		bool bTarget = ArrayElem->ActorHasTag(*targetTag); // FName
		if (bTarget)
		{
			TargetPos = ArrayElem->GetActorLocation();
			Yaw = ArrayElem->GetActorRotation().Yaw;
			return; 
		}
	}
	TargetPos = FVector(0,0,0);
	Yaw = 0.0f;
	return;
}

void ABattleManager::RetrievePlayerPosition(int32 PosIndex, FVector& TargetPos, float& Yaw)
{
	for (auto ArrayElem : PlayerSpawnPoints_Arr)
	{
		FString targetTag;
		targetTag = "p" + FString::FromInt(PosIndex);
		bool bTarget = ArrayElem->ActorHasTag(*targetTag); // FName
		if (bTarget)
		{
			TargetPos = ArrayElem->GetActorLocation();
			Yaw = ArrayElem->GetActorRotation().Yaw;
			return; 
		}
	}
	TargetPos = FVector(0,0,0);
	Yaw = 0.0f;
	return;
}

void ABattleManager::SpawnEnemiesAndDecideLocation()
{
	for (auto It = EnemyTeamInfo.CreateConstIterator(); It; ++It)
	{
		// 按照敌人的类别ID生成战斗对象
		FVector Location;
		FRotator Rotation;
		float yaw;
		RetrieveEnemyPosition(It.Key(), Location, yaw);
		Rotation = FRotator(0,yaw,0);
		TSubclassOf<ABattleEnemy> tempEnemyClass = *EnemyTeamInfo.Find(It.Key());
		ABattleEnemy* enemyTemp = GetWorld()->SpawnActor<ABattleEnemy>(tempEnemyClass, Location, Rotation);
		enemyTemp->PositionID = It.Key();
		Enemies_Arr.AddUnique(enemyTemp);
		
		// 绑定敌人回合结束后的回调
		enemyTemp->OnEnemyTurnEnd.AddDynamic(this, &ABattleManager::EnemyTurnEnd);

		// 绑定敌人被击败后的回调
		enemyTemp->OnEnemyDeath.AddDynamic(this, &ABattleManager::EnemyDeath);
	}
}

void ABattleManager::SpawnPlayersAndDecideLocation()
{
	TeamInstForUI.Empty();

	for (auto It = PlayerTeamInfo.CreateConstIterator(); It; ++It)
	{
		// 按照敌人的类别ID生成战斗对象
		FVector Location;
		FRotator Rotation;
		float yaw;
		RetrievePlayerPosition(It.Key(), Location, yaw);
		Rotation = FRotator(0, yaw, 0);
		FTransform Transform = FTransform(Rotation, Location, FVector(1,1,1));
		TSubclassOf<ABattlePlayer> tempPlayerClass = *PlayerTeamInfo.Find(It.Key());
		if (tempPlayerClass->IsValidLowLevel() && tempPlayerClass)
		{
			ABattlePlayer* playerTemp = GetWorld()->SpawnActorDeferred<ABattlePlayer>(tempPlayerClass, Transform);
			if (playerTemp)
			{
				playerTemp->positionID = It.Key();
				playerTemp->FinishSpawning(Transform);
				Player_Arr.AddUnique(playerTemp); // 进入该数组
				TeamInstForUI.Add(It.Key(), playerTemp);
			}			
		}		
	}
}
