// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/DaisyAIController.h"
#include "Character/DaisyCharacterBase.h"
#include "daisy/daisyEnum.h"
#include "Interface/ExploreInterface.h"
#include "DaisyCharacter.generated.h"

class UDataTable;
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
class DAISY_API ADaisyCharacter : public ADaisyCharacterBase, public IExploreInterface
{
	GENERATED_BODY()
public:
	ADaisyCharacter();
	
	virtual void INT_E_ATK(bool bStart) override;
	
	UPROPERTY(EditAnywhere,Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	TObjectPtr<ADaisyPlayerController> PC = nullptr;

	void FindEnemyInfo(AActor* Enemy);

	UPROPERTY(EditAnywhere,Category = "Data")
	UDataTable* InfoData;

	UPROPERTY(EditAnywhere,Category = "Data")
	FName DataRow = FName("1");
	
	FExplorerInfo explorerInfo;
	UAnimMontage* ATKAnim;
	bool bMelee;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bAttack = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bOpenTeamUI = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool CanMove = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool CanRun = true;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Animation")
	float BlendTime = 0.25;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ShiftMontage;

	float PreMontageSpeed;
	
	UFUNCTION(BlueprintCallable)
	void PlayShiftMontage();
	
	UFUNCTION()
	void OnShiftMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	FTimerHandle ResetBattleBooleanTimerHandle;
	
	void FinishBattle();

	void ResetBattleBoolean();

	//Initialization
	UFUNCTION(BlueprintCallable)
	void SetupNewExplorer(int32 TeamPosIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void PostInitialization();
	
	UFUNCTION(BlueprintImplementableEvent)
	void EnterBattle();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowExploreUI();
	
	UFUNCTION(BlueprintImplementableEvent)
	int32 RetrievCharIndexFromTeamPos(int32 TeamIndex);
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

	void RangeDetectEnemy();
	
private:
};