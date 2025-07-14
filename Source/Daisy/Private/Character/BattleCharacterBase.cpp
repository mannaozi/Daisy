// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BattleCharacterBase.h"

// Sets default values
ABattleCharacterBase::ABattleCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABattleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABattleCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

