// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "daisy/daisyEnum.h"
#include "AN_Attack.generated.h"

/**
 * 
 */
UCLASS()
class DAISY_API UAN_Attack : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ExposeOnSpawn = "true"))
	EAttackType ATKType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ExposeOnSpawn = "true"))
	int32 AttackCountInOneCycle = 0;
};
