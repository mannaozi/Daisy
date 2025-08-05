
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingInicator.generated.h"

UCLASS()
class DAISY_API AFloatingInicator : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloatingInicator();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn="true"))
	float FloatingNum = 0.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn="true"))
	FColor SpecifiedColor = FColor::Blue;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn="true"))
	FVector CurrentLocation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn="true"))
	bool bCriticalHit = false;
};
