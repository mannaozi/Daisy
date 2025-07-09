
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DaisyCharacterBase.generated.h"

UCLASS()
class DAISY_API ADaisyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADaisyCharacterBase();

protected:
	virtual void BeginPlay() override;


};
