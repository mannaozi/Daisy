// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DaisyEnemyCharacter.h"
#include "GameFramework/Actor.h"
#include "daisy/daisyEnum.h"
#include "BattleManager.generated.h"

class ABattlePlayer;
class ADaisyEnemyCharacter;
class ADaisyCharacter;
class ABattleEnemy;
class ABattlePawn;

UCLASS()
class DAISY_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleManager();
	
	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX };
	
	void InitBattle(ADaisyCharacter* Player,ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo);
	void ChangeCameraAndStopMovement();

	void A1_PreInitializeBattle();
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY()
	ADaisyCharacter* Player_World = nullptr;
	UPROPERTY()
	ADaisyEnemyCharacter* Enemy_World = nullptr;
	TMap<int32, TSubclassOf<ABattleEnemy>> EnemyTeamInfo;

	TArray<ABattleEnemy*> Enemies_Arr;
	TArray<ABattleEnemy*> Dead_Enemies_Arr;
	TArray<ABattlePlayer*> Player_Arr;
	TArray<ABattlePlayer*> Dead_Player_Arr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Battle")
	TSubclassOf<ABattlePawn> BattlePawnClass;
	
	UPROPERTY()
	ABattlePawn* BattlePawn;
};
