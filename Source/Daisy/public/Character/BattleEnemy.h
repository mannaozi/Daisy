// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "BattleEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, ABattleEnemy*, enemyRef, AActor*, causerRef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyTurnEnd, ABattleEnemy*, enemyRef);

class AFloatingInicator;
class UHeadBarUI;
/**
 * 
 */
UCLASS()
class DAISY_API ABattleEnemy : public ABattleCharacterBase
{
	GENERATED_BODY()
public:

	ABattleEnemy();

	virtual void Tick(float DeltaTime) override;
	
	void InitializeData();
	
	void UpdateLockIcon(bool bHide);

	void UpdateHeadBar();

	void HandleFX();
	void EnterDialtion();
	void ResetDilation();
	void PlayerCameraShake();
	bool HandleToughness(float ReceivedT,ECombatType CauserCType);
	bool CheckElementATK(ECombatType CauserCType);
	void HandleIndicatorNums(FVector Location,float FloatingNum);
	
	UFUNCTION(BlueprintCallable)
	float PlaySpecificAnim(const FString &AnimKey);
	UFUNCTION(BlueprintCallable)
	void EndEnemyTurnFromBP(ABattleEnemy* ActiveActor);
	
	void EnterStun(int32 DelayTurns);
	void PlayStunVFX();
	void SetDelayedTarget(bool Delay,ABattlePlayer* Target);
	void RecoverFromStun();
	EAttackType ActionDecision(const TArray<ABattlePlayer*> playersRef);
	FString RandomActionByRatio();

	UFUNCTION(BlueprintImplementableEvent)
	void SingleATK(ABattlePlayer* Target);
	UFUNCTION(BlueprintImplementableEvent)
	void RadialATK(const TArray<ABattlePlayer*> &Target);

	UFUNCTION(BlueprintImplementableEvent)
	void ExtraActionWhenStun(bool bEnter);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FEnemyCharAttributes EnemyAtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UDataTable* EnemyCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* HeadBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Presets")
	UParticleSystem* StunVFX;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> NormalCS;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> SkillCS;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> FollowCS;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Presets")
	TSubclassOf<UCameraShakeBase> UltimateCS;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UHeadBarUI> HeadBarUI;
	
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FOnEnemyDeath OnEnemyDeath;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FOnEnemyTurnEnd OnEnemyTurnEnd;
	
	/* Combat Interface */
	virtual void RefreshActionValueBySpd() override;
	virtual void HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;
	virtual void SetATK(EAttackType ATKType, int32 AttackCountInOneCycle) override;
	/* Combat Interface */

	float CurThoughness;
	float MaxThoughness;
	float CurHP;
	float MaxHP;
	float StunVFXHeight;
	float totalATK;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector originLocation;
	TArray<ECombatType> Weaknesses;
	TMap<FString,UAnimMontage*> AnimMontages;
	TMap<FString,FEnemyATKInfo> ValidATKStr;
	TMap<FString,float> choices;
	AActor* DmgCauser;
	float ReceivedHPDmg;
	int32 RecoverFromStunTurns = 0;
	UParticleSystemComponent* StunVFXComp;
	bool bDelayed_ATK = false;
	ABattlePlayer* DelayedTarget;
	
	ABattlePlayer* currentTarget;
	TArray<ABattlePlayer*> currentTargetsArr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BP")
	FString actionAnimKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BP")
	bool bRadialATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BP")
	bool bRangeATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BP")
	int32 ATKDistance;

	
	FTimerHandle DilationHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<AFloatingInicator> FloatingIndicatorClass;

	//Ability
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSigature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSigature OnMaxHealthChanged;

	void Die();
protected:
	virtual void BeginPlay() override;
};
