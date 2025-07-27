// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "Components/TimelineComponent.h"
#include "BattlePlayer.generated.h"

struct FPlayerCharAttributes;
class UCurveFloat;
struct FTimeline;
class AFloatingInicator;
/**
 * 
 */
UCLASS()
class DAISY_API ABattlePlayer : public ABattleCharacterBase
{
	GENERATED_BODY()
	
public:
	ABattlePlayer();

	void UpdateLockIcon(bool bHide);
	
	UPROPERTY(EditAnywhere, Category = "Runtime")
	int32 positionID = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FPlayerCharAttributes playerAtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UDataTable* PlayerCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* MarkedIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TSubclassOf<AFloatingInicator> FloatingIndicatorClass;
	
	/* Combat Interface */
	virtual void RefreshActionValueBySpd() override;
	virtual void SetATK(EAttackType ATKType, int32 AttackCountInOneCycle) override;
	virtual void HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;
	/* Combat Interface */

	EAttackType AttackType {EAttackType::AT_NormalATK};
	
	void InitializeData();
	void SingleAtk(AActor* Target,bool bConsumeTurn,bool bMelee,EAttackType ATKType);
	void MultipleAtk(TArray<AActor*> Target,bool bConsumeTurn,bool bMelee,EAttackType ATKType);
	void PlayAnimationAndTimeLine();

	UFUNCTION(BlueprintCallable)
	void GetPlayerAttributes(float &mHP,float &cHP,float &mEP,float &cEP,float &mT,float &cT);
	//TIMELINE
	UFUNCTION()
	void TL_RotateToTarget(float DeltaTime);
	UFUNCTION()
	void TL_SlideF(float DeltaTime);
	UFUNCTION()
	void TL_SlideF_Finished();
	UFUNCTION()
	void TL_SlideB(float DeltaTime);
	UFUNCTION()
	void TL_SlideB_Finished();
	
	void SetHiddenForPlayer(bool bCustomHidden);
	float PlaySpecifiedAnim(FString Str);
	void PlayATKAnimByATKType();
	void AfterPlayingMeleeATKAnim();
	void GeneralPlayerAttackOver();

	void HandleEP(EAttackType ATKType,bool bDirect,float val);

	void CalculateDmg(bool Buff,float &hpDmg,float &toughnessDmg);

	void SetDelayedMark(bool bNewVisibility);

	void HandleShieldAndHP(float dmg);

	void SpawnFloatingAndPlaySound(FVector loc, float num, FColor col);

	void Healing(float val);
	void AddShieldBuff(float val);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Presets")
	UCurveFloat* Curve_RotateToTarget;

	FTimeline RotateToTarget_Timeline;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Presets")
	UCurveFloat* Curve_Sliding;
	FTimeline Sliding_F_Timeline;
	FTimeline Sliding_B_Timeline;
	FTimerHandle PlayATKAnimHandle;
	FTimerHandle AfterPlayingMeleeATKAnimHandle;
	FTimerHandle MeleePlayerEndHandle;
	
	float MaxHP;
	float CurHp;
	float MaxEnergy;
	float CurEnergy;
	float MaxShield;
	float CurShield;
	FVector OriginLocation;
	FRotator OriginRotation;
	bool ConsumeTurn;
	bool Melee;
	AActor* TargetActor;
	AActor* RotateToTargetActor;
	FVector TargetLocation;
	TArray<AActor*> TargetActors;

	float attackCountInOneCycle;
	FBuffInfo BuffInfo;
	int32 shieldDuration = 0;
	FString shieldTag = FString("tag_shield");
	AActor* shieldGuard = nullptr;
	AActor* DmgCauser = nullptr;
	float receivedDmg = 0.0f;

	FORCEINLINE USpringArmComponent* GetCameraBoom() {return CameraBoom;}
};
