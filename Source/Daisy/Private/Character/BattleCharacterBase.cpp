

#include "Character/BattleCharacterBase.h"
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

UAbilitySystemComponent* ABattleCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABattleCharacterBase::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void ABattleCharacterBase::GetFactionAVAvatar(bool& bPF, float& aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = ActionValue;
	AI = AvatarIcon;
}
