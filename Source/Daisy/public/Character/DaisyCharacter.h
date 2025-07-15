// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/DaisyAIController.h"
#include "Character/DaisyCharacterBase.h"
#include "DaisyCharacter.generated.h"

class ABattlePlayer;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UBehaviorTree;
class UInputAction;
class UInputMappingContext;
class ADaisyPlayerController;
/**
 * 
 */
UCLASS()
class DAISY_API ADaisyCharacter : public ADaisyCharacterBase
{
	GENERATED_BODY()
public:
	ADaisyCharacter();

	UPROPERTY(EditAnywhere,Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere,Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	TObjectPtr<ADaisyPlayerController> PC = nullptr;

	void FindEnemyInfo(AActor* Enemy);

	UPROPERTY(EditDefaultsOnly, Category = "Presets")
	TMap<int32, TSubclassOf<ABattlePlayer>> PlayerTeamInfo;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DaisyContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Attack();

	void Attack_Test();
private:
	bool bOpenTeamUI = false;
	bool bAttack = false;
};
