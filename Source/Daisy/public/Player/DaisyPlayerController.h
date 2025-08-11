
#pragma once

#include "CoreMinimal.h"
#include "Character/BattlePlayer.h"
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

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Presets")
	TMap<int32, TSubclassOf<ABattlePlayer>> PlayerTeamInfo;
protected:
	virtual void BeginPlay() override;
	
};