
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DaisyPlayerController.generated.h"

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
	
};