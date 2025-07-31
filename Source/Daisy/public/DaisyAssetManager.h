// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DaisyAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UDaisyAssetManager& Get();
	virtual void StartInitialLoading() override;
};
