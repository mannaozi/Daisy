// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DaisyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

class AAIController;

ADaisyCharacter::ADaisyCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
}

void ADaisyCharacter::SetControlMode()
{
	if (DaisyAIController) {
		DaisyAIController->UnPossess();
		DaisyAIController->Destroy();
		DaisyAIController = nullptr;
	}
	DaisyAIController = GetWorld()->SpawnActor<ADaisyAIController>(AIControllerClass);
	DaisyAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	DaisyAIController->RunBehaviorTree(BehaviorTree);
	DaisyAIController->Possess(this);
	
}
