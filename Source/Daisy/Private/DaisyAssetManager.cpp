// Fill out your copyright notice in the Description page of Project Settings.


#include "DaisyAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "DaisyGameplayTags.h"

UDaisyAssetManager& UDaisyAssetManager::Get()
{
	check(GEngine)
	
	UDaisyAssetManager* AuraAssetManager = Cast<UDaisyAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UDaisyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FDaisyGameplayTags::InitializeNativeGameplayTags();

	//very important to use Target Data!!!!!  
	UAbilitySystemGlobals::Get().InitGlobalData();
}
