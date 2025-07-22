// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "BattleEnemy.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FEnemyCharAttributes EnemyAtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UDataTable* EnemyCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* HeadBar;

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

protected:
	virtual void BeginPlay() override;
};
