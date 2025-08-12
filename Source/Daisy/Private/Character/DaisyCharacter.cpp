// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DaisyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/DaisyPlayerController.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/DaisyEnemyCharacter.h"
#include "Debug/DebugHelper.h"

class AAIController;

ADaisyCharacter::ADaisyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
}

void ADaisyCharacter::INT_E_ATK(bool bStart)
{
	if (bStart)
	{
		// 是否近战
		if (bMelee)
		{
			Attack_Test();
		}
		else
		{
			RangeDetectEnemy();
		}
	}
	else
	{
		bAttack = false;
	}
}

void ADaisyCharacter::FindEnemyInfo(AActor* Enemy)
{
	ADaisyEnemyCharacter* HitEnemy = Cast<ADaisyEnemyCharacter>(Enemy);
	check(HitEnemy);
	HitEnemy->EnterBattle(this);
}

void ADaisyCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(DaisyContext);
	PC = Cast<ADaisyPlayerController>(Controller);
	if (PC == nullptr) return;
	
	PC->SetInputMode(FInputModeGameOnly());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(DaisyContext,0);

	PostInitialization();
}

void ADaisyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADaisyCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADaisyCharacter::Look);

		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADaisyCharacter::Attack);

	}
}

void ADaisyCharacter::Attack()
{
	if (!bOpenTeamUI && !bAttack)
	{
		bAttack = true;
		if (ATKAnim != nullptr)
		{
			PlayAnimMontage(ATKAnim);
		}
	}
}

void ADaisyCharacter::Attack_Test()
{
	TArray<FHitResult> OutResults;
	FVector Start = GetActorLocation() + GetActorForwardVector() * 150.0f;
	FCollisionObjectQueryParams ObjQueryParams;
	ObjQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape MySphere = FCollisionShape::MakeSphere(150.0f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 

	GetWorld()->SweepMultiByObjectType(OutResults, Start, Start, FQuat(0,0,0,0), ObjQueryParams,
		MySphere, Params);

	// 显示测试球体
	DrawDebugSphere(GetWorld(), Start, 150.0f, 12, FColor::White, false, 5.0f, 0.0f, 4.0f);

	// 若球体碰到敌人ExplorerEnemies，则立刻进入战斗
	for (auto ArrayElem : OutResults)
	{
		AActor* DetectedActor = Cast<ADaisyEnemyCharacter>(ArrayElem.GetActor());
		if (DetectedActor != nullptr)
		{
			bAttack = true;
			EnterBattle();
			FindEnemyInfo(DetectedActor);
			// 跳出函数
			return;
		}
	}
}

void ADaisyCharacter::RangeDetectEnemy()
{
	
}

void ADaisyCharacter::FinishBattle()
{
	// 1.8s后再进入战斗，避免连续进入战斗
	GetWorld()->GetTimerManager().SetTimer(ResetBattleBooleanTimerHandle,
		this, &ADaisyCharacter::ResetBattleBoolean, 1.8f, false);
	ShowExploreUI();
}

void ADaisyCharacter::ResetBattleBoolean()
{
	bAttack = false;
}

void ADaisyCharacter::SetupNewExplorer(int32 TeamPosIndex)
{
	// TBD - 临时（输入序号等于角色序号）
	FString l_charIndex = FString::FromInt(RetrievCharIndexFromTeamPos(TeamPosIndex));
	// 根据输入的序号，切换不同的探索角色
	// 把int32 转化为 FName
	DataRow = FName(*l_charIndex);

	// 读表
	FString s = DataRow.ToString();
	FExplorerInfo* Row = InfoData->FindRow<FExplorerInfo>(DataRow, s, true);
	if (!Row) return;
	explorerInfo = *Row;

	bMelee = explorerInfo.bMelee;
	ATKAnim = explorerInfo.ATKMontage;
	GetMesh()->SetSkeletalMesh(explorerInfo.SKM);
	GetMesh()->SetAnimInstanceClass(explorerInfo.AnimClass);
	
}

void ADaisyCharacter::Move(const FInputActionValue& Value)
{
	if (!bOpenTeamUI && !bAttack)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();
	
		APawn* ControlledPawn = PC->GetPawn();
		if (ControlledPawn != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = ControlledPawn->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
    
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    
			// add movement 
			ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
			ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void ADaisyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	APawn* ControlledPawn = PC->GetPawn();
	if (ControlledPawn != nullptr)
	{
		// add yaw and pitch input to controller
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}