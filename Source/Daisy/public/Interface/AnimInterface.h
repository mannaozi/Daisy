
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnimInterface.generated.h"

UINTERFACE(MinimalAPI)
class UAnimInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DAISY_API IAnimInterface
{
	GENERATED_BODY()

public:
	virtual void SetUltimateReadyVFX(bool bShow) = 0;
};
