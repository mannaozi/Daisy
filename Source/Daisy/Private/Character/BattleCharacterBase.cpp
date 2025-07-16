

#include "Character/BattleCharacterBase.h"

ABattleCharacterBase::ABattleCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
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

void ABattleCharacterBase::GetFactionAVAvatar(bool& bPF, float& aV, UTexture2D* &AI)
{
	bPF = bPlayerFaction;
	aV = ActionValue;
	AI = AvatarIcon;
}