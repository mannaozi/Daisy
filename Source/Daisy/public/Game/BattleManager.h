// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
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
class USoundBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageSigature, float, NewValue,bool,bShow);

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
	void B1b_CalculateActionValue();
	EBattleFlags CheckGameOver(TMap<ABattleEnemy*, float> eArr, TMap <ABattlePlayer*, float> pArr);

	void A2_BattleEnd(EBattleFlags endResult);

	void B2a_HandlePlayerAttack(ABattlePlayer* activePlayerChar);

	void B2b_HandleEnemyAttack(ABattleEnemy* activeEnemyChar);

	void B3_TurnEnd(AActor* EndTurnActor,bool bConsumeTurn);

	void ResetActionValueAndATKType(bool bConsumeTurn,AActor* TempEndActor);
	//Lock
	void SwitchEnemyLockIcon(bool bNext);
	void SetMultipeEnemyLocks();
	bool IsMultipleTargets();
	void ShowEnemyLockIconByIndex(int32 index);
	void ShowEnemyMultipleLockIcons(TArray<ABattleEnemy*> indexRefs);
	void ShowPlayerLockIconByIndex(int32 index);
	void ShowPlayerMultipleLockIcons(TArray<ABattlePlayer*> indexRefs);
	bool NotResurrectSkill();
	void CalculateLockIndex(bool bNext);
	bool IsBuffTarget();
	void SetDeadPlayerLockedIcon();
	void SetPlayerLockedIcons();
	void HideAllLockedIcons();
	void DisplayLockedIconsAndSetTargets();
	void UpdatePlayerLockedIconToMultiple();
	void UpdateEnemyLockedIconToMultiple();
	bool IsMeleeAction();
	void SwitchAndHideOtherPlayerChars(bool bHideOther,ABattlePlayer* ActiveChar);
	
	//Action
	void ExecuteAction(EAttackType ATKType);
	void HandlePlayerATK(EAttackType AttackType);
	void ExecuteUltimate();
	void EnterUltimate(int32 PlayerIndex);
	void ReadyForUltimate(ABattlePlayer* ReadyPlayer);
	void RemoveUltimateTurn(AActor* CharRef);
	void CheckPlayerRevive();
	void StartFollowingATK(AActor* atkTarget);
	//Camera
	void CameraForBuffSelections();
	
	UFUNCTION()
	void EnemyDeath(ABattleEnemy* enemyRef, AActor* causerRef);

	UFUNCTION()
	void EnemyTurnEnd(ABattleEnemy* enemyRef);

	// Battle Handle
	void HandleDelays();
	void BattleEndCameraStartingFade();
	void CleanBattleField();
	void PlayerWin();
	void EnemyWin();
	void ExitGame();
	UFUNCTION(BlueprintCallable)
	void ExitBattle();
	/*Abilitysubsystem*/
	
	/*Abilitysubsystem*/
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
public:
	FString normalCA = "tag_c_start_normal";
	FString fixedCA = "tag_c_boss";
	FString buffCA = "tag_c_buff";
	
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
	TArray<ABattlePlayer*> UltimatePlayerQueue;
	
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

	UPROPERTY(BlueprintReadOnly)
	UBattleUserWidget* BattleLayout;

	EBattleFlags curBattleFlag {EBattleFlags::BF_EMAX};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Battle")
	USoundBase* UnableSFX;
	
	UPROPERTY()
	ABattlePawn* BattlePawn;
	UPROPERTY(BlueprintReadWrite)
	ABattlePlayer* ActivePlayerRef;
	UPROPERTY(BlueprintReadWrite)
	ABattleEnemy* CurrentEnemyTarget;
	UPROPERTY(BlueprintReadWrite)
	ABattlePlayer* CurrentPlayerTarget;
	UPROPERTY(BlueprintReadWrite)
	TArray<ABattleEnemy*> CurrentEnemyTargets;
	UPROPERTY(BlueprintReadWrite)
	TArray<ABattlePlayer*> CurrentPlayerTargets;
	
	AActor* LastClickedActor;
	ACameraActor* BuffCamera;
	FVector BuffCameraOriginLocation;

	int32 IndexForLockedTarget = 2; //初始选择的敌人
	int32 SkillPoints = 2;
	
	bool bTryFollowingATK = false;
	AActor* GuardedChar = nullptr;
	
	FTimerHandle DisplayEnemyTimerHandle;
	FTimerHandle HandleDelaysTimerHandle;
	FTimerHandle BattleEndCameraStartingFadeTimerHandle;
	FTimerHandle CleanBattleFieldTimerHandle;
	FTimerHandle ExitGameTimerHandle;

	float CurrentSpringArmYaw;
	float TargetSpringArmYaw;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> ClearEnergyGameplayEffectClass;
	void ClearEnergy(AActor* ClearTarget);
	//伤害类型颜色映射
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color Mapping")
	TMap<FGameplayTag, FColor> TagColorMap;
	UFUNCTION(BlueprintCallable, Category = "Color Mapping")
	FColor GetColorByTag(const FGameplayTag& Tag) const;

	//StingTOGameplayTag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TMap<FString, FGameplayTag> StringTagMap;
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayTag GetTagByName(const FString& Name) const;
	//EnemyStringToGameplayTag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TMap<FString, FGameplayTag> Enemy_StringTagMap;
	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayTag GetTagByName_Enemy(const FString& Name) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void AbilityEnd();
	
	UPROPERTY(BlueprintReadWrite)
	float Damage_End = 0.0f;

	UPROPERTY(BlueprintAssignable)
	FOnDamageSigature OnDamageChanged;
	void UpdateDamage(bool bShow) const ;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffect();
	void ClickOnCharacter(AActor* ClickedTarget);
	void RetrieveIndex(AActor* ClickedActorForIndex);
	bool IsTarget(AActor* ClickedTarget);
};
