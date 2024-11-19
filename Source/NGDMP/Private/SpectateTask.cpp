// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectateTask.h"
#include "MasterPlayerController.h"
#include "GameplayTagContainer.h"
#include "BaseMarble.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"


// EnterState
EStateTreeRunStatus USpectateTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (not PlayerController)
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());
	
	PlayerController->FIA_Move.AddDynamic(this, &USpectateTask::CameraMovement);
	PlayerController->FIA_Inspect.AddDynamic(this, &USpectateTask::Inspect);
	PlayerController->FIA_MouseLook.AddDynamic(this, &USpectateTask::CameraPan);

	PlayerController->PossessedMarble = nullptr;
	PlayerController->FPossess_Updated.Broadcast(nullptr);
	
	return EStateTreeRunStatus::Running;
}

// ExitState
void USpectateTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	PlayerController->FIA_Move.RemoveDynamic(this, &USpectateTask::CameraMovement);
	PlayerController->FIA_Inspect.RemoveDynamic(this, &USpectateTask::Inspect);
	PlayerController->FIA_MouseLook.RemoveDynamic(this, &USpectateTask::CameraPan);
}

// CameraMovement
void USpectateTask::CameraMovement(FVector3f Input)
{
	APawn* SpectatePawn = PlayerController->GetPawn();
	const FVector Front = SpectatePawn->GetActorForwardVector();
	const FVector Right = SpectatePawn->GetActorRightVector();
	const FVector Up = FVector(0, 0, 1);

	FVector Target_Movement = Front * Input.X + Right * Input.Y + Up * Input.Z;
	Target_Movement.Normalize();
	
	SpectatePawn->AddMovementInput(Target_Movement, CameraSpeed);

}

EStateTreeRunStatus USpectateTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
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
	else {
		HitMarble = nullptr;
	}
	
	if (HitMarble != AimedMarble)
	{		
		AimedMarble = HitMarble; 
		PlayerController->FAim_Updated.Broadcast(AimedMarble);
	}
	
	return EStateTreeRunStatus::Running;
}

void USpectateTask::CameraPan(FVector2f Input)
{
	PlayerController->AddYawInput(Input.X);
	PlayerController->AddPitchInput(Input.Y);
	PlayerController->GetPawn()->SetActorRotation(PlayerController->GetControlRotation());
}


void USpectateTask::Inspect(bool bInspect)
{
	if (not AimedMarble)
		return;

	// the code below results in "node is inactive" error
	// Source code shows that it is because InstanceStorage
	// and CachedOwner are null
	// SendEvent(FStateTreeEvent(
	// 	FGameplayTag::RequestGameplayTag(FName("Inspect.Inspect"))));

	PlayerController->PossessedMarble = AimedMarble;

	UStateTreeComponent* StateTreeComponent = PlayerController->StateTreeComponent;
	StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(
		FGameplayTag::RequestGameplayTag(FName("Inspect.Inspect"))));
}