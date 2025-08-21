// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattlePlayer.h"
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

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshActionOrder(const TArray<ACharacter*> &CharRefs);

	UFUNCTION(BlueprintImplementableEvent)
	void HandleStatsPanelAnimating(ABattlePlayer* PlayerBase,bool bIncrease);

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchATKMode(EAttackType ATKType);
	
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePhaseHint_CPP(ABattlePlayer* Player,EAttackType ATKType);

	UFUNCTION(BlueprintImplementableEvent)
	void HideATKButtons();

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshUItimateOrder(const TArray<ABattlePlayer*> &Players);
	
	UFUNCTION(BlueprintImplementableEvent)
	void HandlePhaseHintEnemy(ABattleEnemy* Player,bool bFixedCamera,EAttackType ATKType);

	UFUNCTION(BlueprintImplementableEvent)
	void BattleOverHint();
	
	UFUNCTION(BlueprintImplementableEvent)
	void FollowATKHint(const FString &charName);

	UFUNCTION(BlueprintImplementableEvent)
	void DisplaySkillPoints(const int32& CharName);

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchUIImage(FPlayerSkillImage Image);
};
