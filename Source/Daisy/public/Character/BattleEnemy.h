// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "BattleEnemy.generated.h"

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
	float PlaySpecificAnim(const FString &AnimKey);
	void EnterStun(int32 DelayTurns);
	void PlayStunVFX();
	void SetDelayedTarget(bool Delay,ABattlePlayer* Target);
	void RecoverFromStun();

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
	
	UHeadBarUI* HeadBarUI;
	
	/* Combat Interface */
	virtual void RefreshActionValueBySpd() override;
	virtual void HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo BuffInfo) override;
	/* Combat Interface */

	float CurThoughness;
	float MaxThoughness;
	float CurHP;
	float MaxHP;
	float StunVFXHeight;
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
	
	FTimerHandle DilationHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<AFloatingInicator> FloatingIndicatorClass;
protected:
	virtual void BeginPlay() override;
};
