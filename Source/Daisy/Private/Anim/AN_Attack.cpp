// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AN_Attack.h"

#include "Interface/CombatInterface.h"

void UAN_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ICombatInterface* Interface = Cast<ICombatInterface>(MeshComp->GetOwner());
	if (Interface)
	{
		Interface->SetATK(ATKType,AttackCountInOneCycle);
	}
}
