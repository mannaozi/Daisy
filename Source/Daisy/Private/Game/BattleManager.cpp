

#include "Game/BattleManager.h"
#include "Character/DaisyCharacter.h"
#include "Character/DaisyEnemyCharacter.h"
#include "Debug/DebugHelper.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABattleManager::InitBattle(ADaisyCharacter* Player, ADaisyEnemyCharacter* Enemy)
{
	Debug::Print("Start to battle !!");
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	
}


