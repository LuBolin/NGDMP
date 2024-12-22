// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonMarbleCenteredTask.h"
#include "MasterPlayerController.h"
#include "StateTreeExecutionContext.h"
#include "ThirdPersonMarbleCenteredTask.h"

// Enter State
EStateTreeRunStatus UFirstPersonMarbleCenteredTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("Entering FirstPersonMarbleCenteredTask"));
	
	if (not PlayerController)
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());

	// SetViewTarget with blend to the marble's camera
	PlayerController->SetViewTargetWithBlend(PlayerController->PossessedMarble, SetViewBlendDuration, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, true);
	bAnimalHidden = false;

	PlayerController->FIA_MainAction.AddDynamic(this, &UFirstPersonMarbleCenteredTask::Ability1);
	PlayerController->FIA_MouseLook.AddDynamic(this, &UFirstPersonMarbleCenteredTask::CameraPan);
	PlayerController->FIA_Escape.AddDynamic(this, &UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask);
	
	return EStateTreeRunStatus::Running;
}

// Exit State
void UFirstPersonMarbleCenteredTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	PossessedMarble->AnimalMesh->SetOwnerNoSee(true);

	PlayerController->FIA_MainAction.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::Ability1);
	PlayerController->FIA_MouseLook.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::CameraPan);
	PlayerController->FIA_Escape.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask);
	
	UE_LOG(LogTemp, Warning, TEXT("Exiting FirstPersonMarbleCenteredTask"));
}

// Tick
EStateTreeRunStatus UFirstPersonMarbleCenteredTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (not bAnimalHidden)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		FVector CameraLocation = CameraManager->GetCameraLocation();
		ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
		FVector MarbleLocation = PossessedMarble->GetActorLocation();
		FVector LocationOffset = CameraLocation - MarbleLocation;
		if (LocationOffset.Size() <= PossessedMarble->Radius)
		{
			PossessedMarble->AnimalMesh->SetOwnerNoSee(true);
			bAnimalHidden = true;
		}
	}
	
	return EStateTreeRunStatus::Running;
}

// Ability1
void UFirstPersonMarbleCenteredTask::Ability1(bool bClicked)
{
	if (bClicked)
		UE_LOG(LogTemp, Display, TEXT("Ability1"));
}

// CameraPan
void UFirstPersonMarbleCenteredTask::CameraPan(FVector2f Input)
{
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	USpringArmComponent* SpringArm = PossessedMarble->CameraSpringArm;
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	
	CurrentRotation.Yaw += Input.X;
	CurrentRotation.Pitch -= Input.Y;
	SpringArm->SetRelativeRotation(CurrentRotation);
}

// ToThirdPersonMarbleCenteredTask
void UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask(bool bPressed)
{
	// position camera diagonally behind the marble
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	FVector TravelDirection = PossessedMarble->GetVelocity();
	if (TravelDirection.Size() == 0.0f)
		TravelDirection = PossessedMarble->CameraSpringArm->GetForwardVector();
	FVector BackwardUp = TravelDirection;
	BackwardUp.Z = 0;
	BackwardUp.Normalize();
	BackwardUp *= -1;
	BackwardUp.Z = 2;
	FVector MarblePosition = PossessedMarble->GetActorLocation();
	PlayerController->SpectatePawn->SetActorLocation(MarblePosition + BackwardUp);
	PlayerController->SendStateTreeEventByTagString("Marble.ThirdPerson");
}