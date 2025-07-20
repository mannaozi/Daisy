// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/DaisyAnimInstance.h"

#include "Character/BattlePlayer.h"

void UDaisyAnimInstance::SetUltimateReadyVFX(bool bShow)
{
	
}

EAttackType UDaisyAnimInstance::GetAttackType() const
{
	return AttackType;
}

void UDaisyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	ABattlePlayer* TempPlayer = Cast<ABattlePlayer>(GetOwningActor());
	if (TempPlayer)
	{
		AttackType = TempPlayer->AttackType;
	}
}
