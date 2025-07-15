// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DaisyUserWidget.h"
#include "BattleUserWidget.generated.h"

class ABattleManager;
/**
 * 
 */
UCLASS()
class DAISY_API UBattleUserWidget : public UDaisyUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDeferred(ABattleManager* BattleMannager);
};
