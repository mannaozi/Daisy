// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DaisyCharacterBase.h"
#include "DaisyEnemyCharacter.generated.h"

class ADaisyCharacter;
class UBehaviorTree;
class ADaisyAIController;
/**
 * 
 */
UCLASS()
class DAISY_API ADaisyEnemyCharacter : public ADaisyCharacterBase
{
	GENERATED_BODY()
public:
	ADaisyEnemyCharacter();

	virtual void PossessedBy(AController* NewController) override;

	void EnterBattle(ADaisyCharacter* Player);

protected:
	UPROPERTY(EditAnywhere,Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<ADaisyAIController> DaisyAIController;
	
};
