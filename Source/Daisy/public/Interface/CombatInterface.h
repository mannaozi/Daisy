#pragma once

#include "CoreMinimal.h"
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
};
