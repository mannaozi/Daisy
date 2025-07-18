// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Interface/AnimInterface.h"
#include "DaisyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UDaisyAnimInstance : public UAnimInstance ,public IAnimInterface
{
	GENERATED_BODY()
public:
	virtual void SetUltimateReadyVFX(bool bShow) override;
};
