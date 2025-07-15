// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "BattleEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API ABattleEnemy : public ABattleCharacterBase
{
	GENERATED_BODY()
public:
	void UpdateLockIcon(bool bHide);
};
