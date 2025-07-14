

#include "Game/BattleManager.h"
#include "Character/DaisyCharacter.h"
#include "Character/BattleEnemy.h"
#include "Character/DaisyEnemyCharacter.h"
#include "Debug/DebugHelper.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BattlePawn.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABattleManager::InitBattle(ADaisyCharacter* Player, ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo)
{
	Player_World = Player;
	Enemy_World = Enemy;
	EnemyTeamInfo = EnemyInfo;
	A1_PreInitializeBattle();

	ChangeCameraAndStopMovement();
}

void ABattleManager::ChangeCameraAndStopMovement()
{
	BattlePawn = Cast<ABattlePawn>(GetWorld()->SpawnActor<AActor>(BattlePawnClass,FVector(0,0,0),FRotator(0,0,0)));
	GetWorld()->GetFirstPlayerController()->Possess(BattlePawn);

	Player_World->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void ABattleManager::A1_PreInitializeBattle()
{
	ProgressPhase = EProgressPhase::PP_A1_PreInitialization;
	
	Enemies_Arr.Empty();
	Dead_Enemies_Arr.Empty();
	Player_Arr.Empty();
	Dead_Player_Arr.Empty();
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	
}


