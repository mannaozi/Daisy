

#include "Player/BattlePawn.h"
#include "Debug/DebugHelper.h"
#include "Player/DaisyPlayerController.h"

ABattlePawn::ABattlePawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattlePawn::TryNormalATK()
{
	Debug::Print("ABattlePawn::TryNormalATK");
}

void ABattlePawn::Destroyed()
{
	Super::Destroyed();
	PC = Cast<ADaisyPlayerController>(GetController());
	if (PC == nullptr) return;
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void ABattlePawn::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<ADaisyPlayerController>(GetController());
	if (PC == nullptr) return;
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}


