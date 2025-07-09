
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DaisyPlayerController.generated.h"

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

protected:
	
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	
};
