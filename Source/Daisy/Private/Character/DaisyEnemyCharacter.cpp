// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DaisyEnemyCharacter.h"
#include "AI/DaisyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Game/BattleManager.h"

ADaisyEnemyCharacter::ADaisyEnemyCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ADaisyEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	DaisyAIController = Cast<ADaisyAIController>(NewController);
	DaisyAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	DaisyAIController->RunBehaviorTree(BehaviorTree);
}

void ADaisyEnemyCharacter::EnterBattle(ADaisyCharacter* Player)
{
	ABattleManager* BattleManager = UDaisyBlueprintFunctionLibrary::FindBattleManager();
	check(BattleManager);
	BattleManager->InitBattle(Player, this);
}
