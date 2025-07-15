// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DaisyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	bool bBOSSFight = false;

	UPROPERTY(EditAnywhere)
	TArray<int32> UnitsLibrary;
};
