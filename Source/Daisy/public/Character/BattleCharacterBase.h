
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "BattleCharacterBase.generated.h"

UCLASS()
class DAISY_API ABattleCharacterBase : public ACharacter,public ICombatInterface
{
	GENERATED_BODY()

public:
	ABattleCharacterBase();

	bool bDead = false;
	bool bStun = false;
	float ActionValue = 0.0f;
	float Distance = 10000.0f;

	bool bPlayerFaction = false;
	UTexture2D* AvatarIcon;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable,BlueprintPure)
	void GetFactionAVAvatar(bool &bPF,float &aV,UTexture2D* &AI);
	
	/* Combat Interface */
	virtual void GetActionValue(float& ActionVal) override;
	virtual void UpdateActionValue(float WinnerVal) override;
	virtual void RefreshActionValueBySpd() override;
	/* Combat Interface */
};
