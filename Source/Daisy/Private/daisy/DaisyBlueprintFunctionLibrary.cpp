// Fill out your copyright notice in the Description page of Project Settings.


#include "daisy/DaisyBlueprintFunctionLibrary.h"
#include "Game/BattleManager.h"
#include "Game/DaisyGameInstance.h"
#include "Kismet/GameplayStatics.h"

ABattleManager* UDaisyBlueprintFunctionLibrary::FindBattleManager()
{
	AActor* Target = UGameplayStatics::GetActorOfClass(GEngine->GameViewport->GetWorld(), ABattleManager::StaticClass());
	ABattleManager* BattleManager = Cast<ABattleManager>(Target);
	return BattleManager;
}

UDaisyGameInstance* UDaisyBlueprintFunctionLibrary::GetGameInstance()
{
	return Cast<UDaisyGameInstance>(UGameplayStatics::GetGameInstance(GEngine->GameViewport->GetWorld()));
}
