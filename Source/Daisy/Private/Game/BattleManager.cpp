

#include "Game/BattleManager.h"
#include "Character/DaisyCharacter.h"
#include "Character/BattleEnemy.h"
#include "Character/DaisyEnemyCharacter.h"
#include "Debug/DebugHelper.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABattleManager::InitBattle(ADaisyCharacter* Player, ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo)
{
	Debug::Print("Start to battle !!"); 
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	
}


