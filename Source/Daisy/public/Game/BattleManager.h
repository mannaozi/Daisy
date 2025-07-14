// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DaisyEnemyCharacter.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"

class ADaisyEnemyCharacter;
class ADaisyCharacter;
class ABattleEnemy;

UCLASS()
class DAISY_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleManager();

	void InitBattle(ADaisyCharacter* Player,ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo);
	
protected:
	virtual void BeginPlay() override;

};
