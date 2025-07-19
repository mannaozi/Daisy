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
	
	/* Combat Interface */
	virtual void RefreshActionValueBySpd() override;
	/* Combat Interface */

	EAttackType AttackType {EAttackType::AT_NormalATK};
	
	void InitializeData();
	void SingleAtk(AActor* Target,bool bConsumeTurn,bool bMelee,EAttackType ATKType);
	void MultipleAtk(TArray<AActor*> Target,bool bConsumeTurn,bool bMelee,EAttackType ATKType);
	void PlayAnimationAndTimeLine();
	UFUNCTION()
	void TL_RotateToTarget(float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Presets")
	UCurveFloat* Curve_RotateToTarget;

	FTimeline RotateToTarget_Timeline;
	
	float MaxHP;
	float CurHp;
	float MaxEnergy;
	float CurEnergy;
	FVector OriginLocation;
	FRotator OriginRotation;
	bool ConsumeTurn;
	bool Melee;
	AActor* TargetActor;
	AActor* RotateToTargetActor;
	FVector TargetLocation;
	TArray<AActor*> TargetActors;
};
