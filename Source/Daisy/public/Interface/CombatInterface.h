#pragma once

#include "CoreMinimal.h"
#include "daisy/daisyEnum.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};


class DAISY_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void GetActionValue(float & ActionVal) = 0;
	virtual void UpdateActionValue(float WinnerVal) = 0;
	virtual void RefreshActionValueBySpd() = 0;
	virtual void SetATK(EAttackType ATKType,int32 AttackCountInOneCycle) = 0;
	virtual void HitHandle(AActor* causer,float HP_Dmg,float Toughness_Dmg,FBuffInfo BuffInfo) = 0;
	virtual void CountBuffsTimer() = 0;
	virtual void TryFollowingATK() = 0;
};
