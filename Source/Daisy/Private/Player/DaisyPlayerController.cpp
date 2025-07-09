// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DaisyPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"

ADaisyPlayerController::ADaisyPlayerController()
{
	bReplicates = true;
}

void ADaisyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(DaisyContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(DaisyContext,0);
	
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

