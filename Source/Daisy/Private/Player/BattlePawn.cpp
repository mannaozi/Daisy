

#include "Player/BattlePawn.h"

#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Debug/DebugHelper.h"
#include "Game/BattleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DaisyPlayerController.h"

ABattlePawn::ABattlePawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattlePawn::TryAction(EAttackType ATKType)
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->ExecuteAction(ATKType);
}

void ABattlePawn::TrySwitchingEnemyLockIcon(bool bNext)
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->SwitchEnemyLockIcon(bNext);
}

void ABattlePawn::TryCastingUltimate()
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->ExecuteUltimate();
}

void ABattlePawn::TryEnterUltimate(int32 Index)
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->EnterUltimate(Index);
}

void ABattlePawn::TryExecuteAction(EAttackType ATKType)
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->ExecuteAction(ATKType);
}

void ABattlePawn::Apply()
{
	UDaisyBlueprintFunctionLibrary::FindBattleManager()->ApplyEffect();
}

void ABattlePawn::Destroyed()
{
	Super::Destroyed();
	if (PC == nullptr) return;
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void ABattlePawn::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<ADaisyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC == nullptr) return;
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}


