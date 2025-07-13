// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DaisyBlueprintFunctionLibrary.generated.h"

class ABattleManager;
/**
 * 
 */
UCLASS()
class DAISY_API UDaisyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static ABattleManager* FindBattleManager();
};
