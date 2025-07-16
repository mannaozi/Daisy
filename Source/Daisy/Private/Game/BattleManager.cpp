

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

EBattleFlags ABattleManager::CheckGameOver(TMap<ABattleEnemy*, float> eArr, TMap<ABattlePlayer*, float> pArr)
{
	// 若敌人数组为0，则玩家胜利
	if(eArr.Num() == 0) return EBattleFlags::BF_PlayerWin;
	// 若玩家数组为0，则敌人胜利
	if(pArr.Num() == 0) return EBattleFlags::BF_EnemyWin;
	// 否则继续战斗循环
	return EBattleFlags::BF_ContinueBattle;
}

void ABattleManager::A2_BattleEnd(EBattleFlags endResult)
{
	
}

void ABattleManager::B2a_HandlePlayerAttack(ABattlePlayer* activePlayerChar)
{
	Debug::Print("Player Turn !!");
}

void ABattleManager::B2b_HandleEnemyAttack(ABattleEnemy* activeEnemyChar)
{
	Debug::Print("Enemy Turn !!");
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
