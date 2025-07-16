// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DaisyEnemyCharacter.h"
#include "GameFramework/Actor.h"
#include "daisy/daisyEnum.h"
#include "BattleManager.generated.h"

class UBattleUserWidget;
class ABattlePlayer;
class ADaisyEnemyCharacter;
class ADaisyCharacter;
class ABattleEnemy;
class ABattlePawn;
class ACameraActor;
class UUserWidget;

UCLASS()
class DAISY_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABattleManager();
	
	EProgressPhase ProgressPhase{ EProgressPhase::PP_EMAX };
	
	void InitBattle(ADaisyCharacter* Player,ADaisyEnemyCharacter* Enemy,TMap<int32,TSubclassOf<ABattleEnemy>> EnemyInfo);
	void ChangeCameraAndStopMovement();
	
	ACameraActor* RetrieveCamera(FName Tag);

	void RetrieveEnemyPosition(int32 PosIndex, FVector &TargetPos, float &Yaw);
	void RetrievePlayerPosition(int32 PosIndex, FVector &TargetPos, float &Yaw);
	void SpawnEnemiesAndDecideLocation();
	void SpawnPlayersAndDecideLocation();
	
	void A1_PreInitializeBattle();
	void PostInitializeBattle();

	void B1a_CalculateActionValue();
	EBattleFlags CheckGameOver(TMap<ABattleEnemy*, float> eArr, TMap <ABattlePlayer*, float> pArr);

	void A2_BattleEnd(EBattleFlags endResult);

	void B2a_HandlePlayerAttack(ABattlePlayer* activePlayerChar);

	void B2b_HandleEnemyAttack(ABattleEnemy* activeEnemyChar);
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY()
	ADaisyCharacter* Player_World = nullptr;
	UPROPERTY()
	ADaisyEnemyCharacter* Enemy_World = nullptr;
	TMap<int32, TSubclassOf<ABattleEnemy>> EnemyTeamInfo;
	TMap<int32, TSubclassOf<ABattlePlayer>> PlayerTeamInfo;
	
	
	TArray<ABattleEnemy*> Enemies_Arr;
	TArray<ABattleEnemy*> Dead_Enemies_Arr;
	TArray<ABattlePlayer*> Player_Arr;
	TArray<ABattlePlayer*> Dead_Player_Arr;
	
	TArray<ACameraActor*> Cameras_Arr;
	TArray<AActor*> EnemySpawnPoints_Arr;
	TArray<AActor*> PlayerSpawnPoints_Arr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Battle")
	TSubclassOf<ABattlePawn> BattlePawnClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Battle")
	TMap<int32, ABattlePlayer*> TeamInstForUI;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Battle")
	float EnemyDisplayTime = 2.0f;
	UPROPERTY(EditAnywhere,Category="Battle")
	TSubclassOf<UUserWidget> BattleLayoutClass;
	
	UBattleUserWidget* BattleLayout;
	
	UPROPERTY()
	ABattlePawn* BattlePawn;

	FTimerHandle DisplayEnemyTimerHandle;
};
