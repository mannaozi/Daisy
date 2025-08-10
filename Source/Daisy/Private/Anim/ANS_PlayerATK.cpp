// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/ANS_PlayerATK.h"

#include "Interface/ExploreInterface.h"


void UANS_PlayerATK::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	//Super::NotifyBegin()
	IExploreInterface* tempInterface = Cast<IExploreInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->INT_E_ATK(true);
	}
}

void UANS_PlayerATK::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	IExploreInterface* tempInterface = Cast<IExploreInterface>(MeshComp->GetOwner());
	if (tempInterface)
	{
		tempInterface->INT_E_ATK(false);
	}
}