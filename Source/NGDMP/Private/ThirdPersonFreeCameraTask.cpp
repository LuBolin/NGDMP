// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonFreeCameraTask.h"
#include "MasterPlayerController.h"
#include "BaseMarble.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"


// EnterState
EStateTreeRunStatus UThirdPersonFreeCameraTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("Entering ThirdPersonFreeCameraTask"));
	
	if (not PlayerController)
	{
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());
	}
	
	PlayerController->FIA_Move.AddDynamic(this, &UThirdPersonFreeCameraTask::CameraMovement);
	PlayerController->FIA_MouseLook.AddDynamic(this, &UThirdPersonFreeCameraTask::CameraPan);
	PlayerController->FIA_Interact.AddDynamic(this, &UThirdPersonFreeCameraTask::PossessAimedPawn);
	PlayerController->FIA_Toggle.AddDynamic(this, &UThirdPersonFreeCameraTask::ToThirdPersonMarbleCenteredTask);
	
	PlayerController->PossessedMarble = nullptr;
	// Crashes due to nullptr due to order of BeginPlay execution
	// We don't need to run this anyways, but leave this as an example
	// incase we need to do init things in the future
	// PlayerController->FPossess_Updated.Broadcast(nullptr);

	// Sync with pawn rotation
	PlayerController->SetControlRotation(PlayerController->GetPawn()->GetActorRotation());
	
	return EStateTreeRunStatus::Running;
}

// ExitState
void UThirdPersonFreeCameraTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	PlayerController->FIA_Move.RemoveDynamic(this, &UThirdPersonFreeCameraTask::CameraMovement);
	PlayerController->FIA_MouseLook.RemoveDynamic(this, &UThirdPersonFreeCameraTask::CameraPan);
	PlayerController->FIA_Interact.RemoveDynamic(this, &UThirdPersonFreeCameraTask::PossessAimedPawn);
	PlayerController->FIA_Toggle.RemoveDynamic(this, &UThirdPersonFreeCameraTask::ToThirdPersonMarbleCenteredTask);

	UE_LOG(LogTemp, Warning, TEXT("Exiting ThirdPersonFreeCameraTask"));
}

// CameraMovement
void UThirdPersonFreeCameraTask::CameraMovement(FVector3f Input)
{
	APawn* SpectatePawn = PlayerController->GetPawn();
	const FVector Front = SpectatePawn->GetActorForwardVector();
	const FVector Right = SpectatePawn->GetActorRightVector();
	const FVector Up = FVector(0, 0, 1);

	FVector Target_Movement = Front * Input.X + Right * Input.Y + Up * Input.Z;
	Target_Movement.Normalize();
	
	SpectatePawn->AddMovementInput(Target_Movement, CameraSpeed);

}

EStateTreeRunStatus UThirdPersonFreeCameraTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// line trace for 'aim' update
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector CameraForward = CameraManager->GetCameraRotation().Vector();
	FVector TraceEnd = CameraLocation + CameraForward * 1000.0f;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerController->GetPawn());
	bool bHit = PlayerController->GetWorld()->LineTraceSingleByChannel(
		HitResult, CameraLocation, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);

	ABaseMarble* HitMarble = nullptr;
	if (bHit)
	{
		// if failed cast, auto nullptr
		HitMarble = Cast<ABaseMarble>(HitResult.GetActor());
	}
	
	if (HitMarble != AimedMarble)
	{		
		AimedMarble = HitMarble; 
		PlayerController->FAim_Updated.Broadcast(AimedMarble);
	}
	
	return EStateTreeRunStatus::Running;
}

void UThirdPersonFreeCameraTask::CameraPan(FVector2f Input)
{
	PlayerController->AddYawInput(Input.X);
	PlayerController->AddPitchInput(Input.Y);
	PlayerController->GetPawn()->SetActorRotation(PlayerController->GetControlRotation());
}

void UThirdPersonFreeCameraTask::PossessAimedPawn(bool bInspect)
{
	if (not AimedMarble)
	{
		return;
	}

	PlayerController->PossessedMarble = AimedMarble;
	ToThirdPersonMarbleCenteredTask(bInspect);
}

void UThirdPersonFreeCameraTask::ToThirdPersonMarbleCenteredTask(bool bInspect)
{
	if (not PlayerController->PossessedMarble)
	{
		return;
	}
	
	// FinishTask(true); // why does this not trigger the transition?
	PlayerController->SendStateTreeEventByTagString("Marble.ThirdPerson");
}