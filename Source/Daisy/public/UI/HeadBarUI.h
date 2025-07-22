// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "daisy/daisyEnum.h"
#include "UI/DaisyUserWidget.h"
#include "HeadBarUI.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UHeadBarUI : public UDaisyUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnemyHeadBar(float CurHP,float CurThoughness,float MaxHP,float MaxThoughness,const TArray<ECombatType> &Weaknesses);
};
