// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/DaisyAIController.h"
#include "Character/DaisyCharacterBase.h"
#include "DaisyCharacter.generated.h"

class UBehaviorTree;
/**
 * 
 */
UCLASS()
class DAISY_API ADaisyCharacter : public ADaisyCharacterBase
{
	GENERATED_BODY()
public:
	ADaisyCharacter();
	
	bool IsPlayerCharacter;
	
	UFUNCTION(BlueprintCallable)
	void SetControlMode();

	UPROPERTY(EditAnywhere,Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<ADaisyAIController> DaisyAIController;
	
};
