

#include "Character/BattleCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DaisyGameplayTags.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystem/DaisyAbilitySystemComponent.h"
#include "AbilitySystem/DaisyAttributeSet.h"


ABattleCharacterBase::ABattleCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("Follow Camera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	LockIcon = CreateDefaultSubobject<UWidgetComponent>("Lock");
	LockIcon->SetupAttachment(RootComponent);
	LockIcon->bHiddenInGame = true;
}

void ABattleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABattleCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABattleCharacterBase::GetActionValue(float& ActionVal)
{
	ActionVal = ActionValue;
}

void ABattleCharacterBase::UpdateActionValue(float WinnerVal)
{
	ActionValue -= WinnerVal;
}

void ABattleCharacterBase::RefreshActionValueBySpd()
{
	
}

void ABattleCharacterBase::SetATK(EAttackType ATKType, int32 AttackCountInOneCycle)
{
	
}

void ABattleCharacterBase::HitHandle(AActor* causer, float HP_Dmg, float Toughness_Dmg, FBuffInfo buff_Info)
{
	
}

void ABattleCharacterBase::CountBuffsTimer()
{
	
}

void ABattleCharacterBase::TryFollowingATK()
{
	
}

UAnimMontage* ABattleCharacterBase::GetDodgeMontage_Implementation()
{
	return nullptr;
}

UAnimMontage* ABattleCharacterBase::GetBlockMontage_Implementation()
{
	return nullptr;
}

UAbilitySystemComponent* ABattleCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ABattleCharacterBase::GetHitReackMontage()
{
	return HitReactMontage;
}

void ABattleCharacterBase::InitializeDefaultAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()))
	check(DefaultAttributes);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultAttributes,Level,ContextHandle);
	
	FDaisyGameplayTags GameplayTags = FDaisyGameplayTags::Get();
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Health,InitAttribute.Health);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_MaxHealth,InitAttribute.MaxHealth);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Shield,InitAttribute.Shield);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Energy,InitAttribute.Energy);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_MaxEnergy,InitAttribute.MaxEnergy);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Attack,InitAttribute.Attack);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Defense,InitAttribute.Defense);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Critical,InitAttribute.Critical);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_CriticalDamage,InitAttribute.CriticalDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Speed,InitAttribute.Speed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Level,InitAttribute.Level);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_Toughness,InitAttribute.Toughness);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attribute_MaxToughness,InitAttribute.MaxToughness);
	
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),GetAbilitySystemComponent());
}

void ABattleCharacterBase::AddCharacterAbilities()
{
	UDaisyAbilitySystemComponent* AuraASC = Cast<UDaisyAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(StartupAbilities);
}

void ABattleCharacterBase::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

TArray<FBuffList> ABattleCharacterBase::CreateBuffList()
{
	TArray<FBuffList> BuffList;
	for (auto Pair : BuffMap)
	{
		BuffList.Add(Pair.Value);
	}
	return BuffList;
}

void ABattleCharacterBase::GetFactionAVAvatar(bool& bPF, float& aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = ActionValue;
	AI = AvatarIcon;
}
