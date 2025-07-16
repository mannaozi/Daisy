// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "BattleEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API ABattleEnemy : public ABattleCharacterBase
{
	GENERATED_BODY()
public:

	ABattleEnemy();
	
	void UpdateLockIcon(bool bHide);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FEnemyCharAttributes EnemyAtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UDataTable* EnemyCharsDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName DataRow = FName("1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	UWidgetComponent* HeadBar;
	
	/* Combat Interface */
	virtual void RefreshActionValueBySpd() override;
	/* Combat Interface */

protected:
	virtual void BeginPlay() override;
};
