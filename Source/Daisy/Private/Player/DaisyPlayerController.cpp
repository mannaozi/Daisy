// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DaisyPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Character/DaisyCharacter.h"

ADaisyPlayerController::ADaisyPlayerController()
{
	bReplicates = true;
}

void ADaisyPlayerController::SwitchToCharacter(int32 Index)
{
	if (TeamMembers.IsValidIndex(Index) && TeamMembers[Index])
	{
		// 解除当前控制
		if (GetPawn()) {
			Cast<ADaisyCharacter>(GetPawn())->SetControlMode();
		}
		// 获取目标角色
		APawn* TargetPawn = Cast<APawn>(TeamMembers[Index]);
		if (TargetPawn)
		{
			AAIController* OldAIController = Cast<AAIController>(TargetPawn->GetController());
			if (OldAIController)
			{
				OldAIController->UnPossess();
				OldAIController->Destroy();
			}
		}
		// 控制新角色
		Possess(Cast<APawn>(TeamMembers[Index]));
        
		SetViewTargetWithBlend(Cast<AActor>(TeamMembers[Index]),1.0f, VTBlend_EaseOut, 0.5f);
		
	}
}

void ADaisyPlayerController::InitializeCharacter()
{
	
	// 查找所有生成点
    TArray<AActor*> SpawnPoints;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PlayerSpawn"), SpawnPoints);
    // 清空小队成员数组
    TeamMembers.Empty();
	int32 j = SpawnPoints.Num();
    // 生成小队成员
    for (int32 i = 0; i < j; i++)
    {
        if (CharacterClasses[i])
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            
            ADaisyCharacter* NewCharacter = GetWorld()->SpawnActor<ADaisyCharacter>(
                CharacterClasses[i],
                SpawnPoints[i]->GetActorLocation(),
                SpawnPoints[i]->GetActorRotation(),
                SpawnParams
            );
            if (NewCharacter)
            {
                TeamMembers.Add(NewCharacter);
            }
        	if (i != 0) { // 假设0是玩家初始角色
        		NewCharacter->SetControlMode();
        	}
        }
    }
    // 控制第一个角色
    if (TeamMembers.Num() > 0)
    {
        Possess(Cast<APawn>(TeamMembers[0]));
        SetViewTarget(Cast<APawn>(TeamMembers[0]));
    }
	
}

void ADaisyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(DaisyContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(DaisyContext,0);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		//InitializeCharacter();
	}, 0.5f, false);
}

void ADaisyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADaisyPlayerController::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADaisyPlayerController::Look);

		// Looking
		EnhancedInputComponent->BindAction(SwitchTo1, ETriggerEvent::Started, this, &ADaisyPlayerController::SwitchToCharacter,0);

		// Looking
		EnhancedInputComponent->BindAction(SwitchTo2, ETriggerEvent::Started, this, &ADaisyPlayerController::SwitchToCharacter,1);

		// Looking
		EnhancedInputComponent->BindAction(SwitchTo3, ETriggerEvent::Started, this, &ADaisyPlayerController::SwitchToCharacter,2);

		// Looking
		EnhancedInputComponent->BindAction(SwitchTo4, ETriggerEvent::Started, this, &ADaisyPlayerController::SwitchToCharacter,3);

	}
}

void ADaisyPlayerController::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();
	
	APawn* ControlledPawn = GetPawn();
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

void ADaisyPlayerController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		// add yaw and pitch input to controller
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

