
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "Interface/VFXInterface.h"
#include "GameplayEffectTypes.h"
#include "BattleCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UDaisyAttributeSet;
class UDaisyAbilitySystemComponent;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSigature, float, NewValue);
UCLASS()
class DAISY_API ABattleCharacterBase : public ACharacter,public ICombatInterface,public IVFXInterface,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABattleCharacterBase();

	bool bDead = false;
	bool bStun = false;
	UPROPERTY(BlueprintReadWrite)
	float ActionValue = 0.0f;
	float Distance = 10000.0f;

	bool bPlayerFaction = false;
	UTexture2D* AvatarIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> LockIcon;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:	

	UFUNCTION(BlueprintCallable,BlueprintPure)
	void GetFactionAVAvatar(bool &bPF,float &aV,UTexture2D* &AI);
	
	/* Combat Interface */
	virtual void GetActionValue(float& ActionVal) override;
	virtual void UpdateActionValue(float WinnerVal) override;
	virtual void RefreshActionValueBySpd() override;
	virtual void SetATK(EAttackType ATKType,int32 AttackCountInOneCycle) override;
	virtual void HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info) override;
	virtual void CountBuffsTimer() override;
	virtual void TryFollowingATK() override;
	/* Combat Interface */

	/* Abilitysystem */
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributes;
	
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetHitReackMontage();
	
	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();
	
	/* Abilitysystem */

	int32 Level = 1;
	virtual void InitAbilityActorInfo();

	//Buff
	UPROPERTY(BlueprintReadWrite)
	TMap<FActiveGameplayEffectHandle,int32> BuffMap;
};
