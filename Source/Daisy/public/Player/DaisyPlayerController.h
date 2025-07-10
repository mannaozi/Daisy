
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DaisyPlayerController.generated.h"

class ADaisyCharacter;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class DAISY_API ADaisyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ADaisyPlayerController();

	UPROPERTY(BlueprintReadWrite, Category = "Team")
	TArray<ADaisyCharacter*> TeamMembers; // 存储小队成员

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	TArray<TSubclassOf<ADaisyCharacter>> CharacterClasses;

	UFUNCTION(BlueprintCallable)
	void SwitchToCharacter(int32 Index); // 切换控制函数

	void InitializeCharacter();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
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
	TObjectPtr<UInputAction> SwitchTo1;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SwitchTo2;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SwitchTo3;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SwitchTo4;
protected:
	
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	
};
