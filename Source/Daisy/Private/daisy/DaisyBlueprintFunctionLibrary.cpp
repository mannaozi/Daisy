// Fill out your copyright notice in the Description page of Project Settings.


#include "daisy/DaisyBlueprintFunctionLibrary.h"

#include "DaisyAbilityTypes.h"
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

bool UDaisyBlueprintFunctionLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDaisyGameplayEffectContext* DaisyEffectContext = static_cast<const FDaisyGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DaisyEffectContext->IsCriticalHit();
	}
	return false;
}

void UDaisyBlueprintFunctionLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInCriticalHit)
{
	if (FDaisyGameplayEffectContext* DaisyEffectContext = static_cast<FDaisyGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DaisyEffectContext->SetIsCriticalHit(bInCriticalHit);
	}
}

FColor UDaisyBlueprintFunctionLibrary::GetDamageColor(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FDaisyGameplayEffectContext* DaisyEffectContext = static_cast<const FDaisyGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return DaisyEffectContext->GetColor();
	}
	return FColor::Red;
}

void UDaisyBlueprintFunctionLibrary::SetDamageColor(FGameplayEffectContextHandle& EffectContextHandle, FColor InColor)
{
	if (FDaisyGameplayEffectContext* DaisyEffectContext = static_cast<FDaisyGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		DaisyEffectContext->SetColor(InColor);
	}
}
