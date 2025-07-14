
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BattlePawn.generated.h"

class ADaisyPlayerController;

UCLASS()
class DAISY_API ABattlePawn : public APawn
{
	GENERATED_BODY()

public:
	ABattlePawn();

	UFUNCTION(BlueprintCallable)
	void TryNormalATK();
	
	UPROPERTY()
	TObjectPtr<ADaisyPlayerController> PC; 
protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
};
