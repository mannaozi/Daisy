

#include "Character/BattleCharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"


ABattleCharacterBase::ABattleCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

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

void ABattleCharacterBase::GetFactionAVAvatar(bool& bPF, float& aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = ActionValue;
	AI = AvatarIcon;
}