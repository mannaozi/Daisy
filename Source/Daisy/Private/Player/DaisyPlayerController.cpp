// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DaisyPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

ADaisyPlayerController::ADaisyPlayerController()
{
	bReplicates = true;
}


void ADaisyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}
