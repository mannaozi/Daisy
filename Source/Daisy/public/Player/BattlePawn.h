
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BattlePawn.generated.h"

enum class EAttackType : uint8;
class ADaisyPlayerController;

UCLASS()
class DAISY_API ABattlePawn : public APawn
{
	GENERATED_BODY()

public:
	ABattlePawn();

	UFUNCTION(BlueprintCallable)
	void TryAction(EAttackType ATKType);

	UFUNCTION(BlueprintCallable)
	void TrySwitchingEnemyLockIcon(bool bNext);

	UFUNCTION(BlueprintCallable)
	void TryCastingUltimate();

	UFUNCTION(BlueprintCallable)
	void TryEnterUltimate(int32 Index);

	UFUNCTION(BlueprintCallable)
	void TryExecuteAction(EAttackType ATKType);

	UFUNCTION(BlueprintCallable)
	void Apply();

	UFUNCTION(BlueprintCallable)
	void TryClickOnCharacter(AActor* ClickedTarget);
	
	UPROPERTY()
	TObjectPtr<ADaisyPlayerController> PC; 
protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
};
