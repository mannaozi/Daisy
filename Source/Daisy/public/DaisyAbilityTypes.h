#pragma once
#include "GameplayEffectTypes.h"
#include "DaisyAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FDaisyGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	bool IsCriticalHit() const {return bIsCriticalHit;}
	FColor GetColor() const {return Color;}
	
	
	void SetIsCriticalHit(bool bIsCritical) {bIsCriticalHit = bIsCritical;}
	void SetColor(const FColor InColor) {Color = InColor;}

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FGameplayEffectContext* Duplicate() const override
	{
		FGameplayEffectContext* NewContext = new FGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	FColor Color = FColor::White;
	
};
template<>
struct TStructOpsTypeTraits< FDaisyGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FDaisyGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};