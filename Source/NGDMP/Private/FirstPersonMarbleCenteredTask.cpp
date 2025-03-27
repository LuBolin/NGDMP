// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonMarbleCenteredTask.h"
#include "MasterPlayerController.h"
#include "StateTreeExecutionContext.h"
#include "ThirdPersonMarbleCenteredTask.h"

// Enter State
EStateTreeRunStatus UFirstPersonMarbleCenteredTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	// SetViewTarget with blend to the marble's camera
	PlayerController->SetViewTargetWithBlend(PlayerController->PossessedMarble, SetViewBlendDuration, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, true);
	bAnimalHidden = false;

	PlayerController->FIA_MouseLook.AddDynamic(this, &UFirstPersonMarbleCenteredTask::CameraPan);
	PlayerController->FIA_Escape.AddDynamic(this, &UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask);
	PlayerController->FIA_MainAction.AddDynamic(this, &UFirstPersonMarbleCenteredTask::UseAbility);


	GEngine->GameViewport->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
	PlayerController->bShowMouseCursor = false;
	
	return EStateTreeRunStatus::Running;
}

// Exit State
void UFirstPersonMarbleCenteredTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	PossessedMarble->AnimalMesh->SetOwnerNoSee(false);
	PossessedMarble->StatusLabel->SetOwnerNoSee(false);

	PlayerController->FIA_MouseLook.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::CameraPan);
	PlayerController->FIA_Escape.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask);
	PlayerController->FIA_MainAction.RemoveDynamic(this, &UFirstPersonMarbleCenteredTask::UseAbility);
	
	Super::ExitState(Context, Transition);
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
			PossessedMarble->StatusLabel->SetOwnerNoSee(true);
			bAnimalHidden = true;
		}
	}
	
	return EStateTreeRunStatus::Running;
}


// CameraPan
void UFirstPersonMarbleCenteredTask::CameraPan(FVector2f Input)
{
	// multiply by delta time, then multiply by 60
	// values were tuned at 60fps
	float deltaTime = PlayerController->GetWorld()->GetDeltaSeconds();
	Input *= deltaTime * 60.0f;
	
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	USpringArmComponent* SpringArm = PossessedMarble->AnimalCameraSpringArm;
	FRotator CurrentRotation = SpringArm->GetRelativeRotation();
	
	CurrentRotation.Yaw += Input.X;
	CurrentRotation.Pitch -= Input.Y;
	SpringArm->SetRelativeRotation(CurrentRotation);
}


void UFirstPersonMarbleCenteredTask::ToThirdPersonMarbleCenteredTask(bool bPressed)
{
	// position camera diagonally behind the marble
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	// FVector TravelDirection = PossessedMarble->GetVelocity();
	// if (TravelDirection.Size() == 0.0f)
	// 	TravelDirection = PossessedMarble->AnimalCameraSpringArm->GetForwardVector();
	// FVector BackwardUp = TravelDirection;
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	FVector CameraDirection = CameraManager->GetCameraRotation().Vector();
	FVector BackwardUp = CameraDirection;
	BackwardUp.Z = 0;
	BackwardUp.Normalize();
	BackwardUp *= -1;
	BackwardUp.Z = 1;
	BackwardUp *= UThirdPersonMarbleCenteredTask::MinCamDist;
	FVector MarblePosition = PossessedMarble->GetActorLocation();
	PlayerController->SpectatePawn->SetActorLocation(MarblePosition + BackwardUp);
	PlayerController->SendStateTreeEventByTagString("Marble.ThirdPerson");
}

void UFirstPersonMarbleCenteredTask::UseAbility(bool bPressed)
{
	ABaseMarble* Marble = PlayerController->PossessedMarble;
	Marble->UseAbility(true);
	float PopOutDelay = Marble->AnimalDataAsset->AbilityCameraTransitionDelay;
	if (PopOutDelay < 0.0f) // meaning "do not pop out"
	{
		return;
	}
	// pop out into third person after a delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		ToThirdPersonMarbleCenteredTask(true);
	}, PopOutDelay, false);
}