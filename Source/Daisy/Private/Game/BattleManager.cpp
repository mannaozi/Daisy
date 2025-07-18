

#include "Game/BattleManager.h"
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


ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABattleManager::InitBattle(ADaisyCharacter* Player, ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo)
{
	Player_World = Player;
	Enemy_World = Enemy;
	EnemyTeamInfo = EnemyInfo;
	PlayerTeamInfo = Player->PlayerTeamInfo;

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
		}
	}
}

void ABattleManager::A2_BattleEnd(EBattleFlags endResult)
{
	
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
			//TODO 被消灭敌人解绑函数
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
	EBattleFlags curBattleFlag = EBattleFlags::BF_EMAX;
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

void ABattleManager::B2a_HandlePlayerAttack(ABattlePlayer* activePlayerChar)
{
	ActivePlayerRef = activePlayerChar;

	ProgressPhase = EProgressPhase::PP_B2a_PlayerActionTime;
	
	DisplayLockedIconsAndSetTargets();
}

void ABattleManager::B2b_HandleEnemyAttack(ABattleEnemy* activeEnemyChar)
{
	Debug::Print("Enemy Turn !!");
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
		ShowEnemyMultipleLockIcons(CurrentEnemyTargets);
	}
	else
	{
		CurrentEnemyTarget = Enemies_Arr[IndexForLockedTarget];
		LastClickedActor = CurrentEnemyTarget;
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
			IndexForLockedTarget = (Dead_Player_Arr.Num()-1) / 2;
		}
		else
		{
			IndexForLockedTarget = -1;
		}
		if (!Dead_Player_Arr.IsValidIndex(IndexForLockedTarget)) return;
		if (Dead_Player_Arr[IndexForLockedTarget])
		{
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

		//UI文字提示
		BattleLayout->HandlePhaseHint_CPP(ActivePlayerRef,AttackType);
		//战斗中隐藏UI
		BattleLayout->HideATKButtons();
		
	}
}

void ABattleManager::ExecuteUltimate()
{
	
}

void ABattleManager::CameraForBuffSelections()
{
	
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
	FString normalCA = "tag_c_start_normal";
	FString fixedCA = "tag_c_boss";
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
		Enemies_Arr.AddUnique(enemyTemp); 

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
