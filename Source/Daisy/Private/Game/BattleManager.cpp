

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
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Game/DaisyGameInstance.h"

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
	
	A1_PreInitializeBattle();

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
