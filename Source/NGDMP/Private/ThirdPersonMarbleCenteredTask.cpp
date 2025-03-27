// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMarbleCenteredTask.h"
#include "MasterPlayerController.h"
#include "MyGameModeBase.h"
#include "PrimaryHUD.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateTreeExecutionContext.h"
#include "TurnBasedGameState.h"
#include "Kismet/GameplayStatics.h"

// EnterState
EStateTreeRunStatus UThirdPersonMarbleCenteredTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	
	PlayerController->FIA_Move.AddDynamic(this, &UThirdPersonMarbleCenteredTask::CameraMovement);
	PlayerController->FIA_Escape.AddDynamic(this, &UThirdPersonMarbleCenteredTask::ToThirdPersonFreeCameraTask);
	PlayerController->FIA_Toggle.AddDynamic(this, &UThirdPersonMarbleCenteredTask::ToFirstPersonMarbleCenteredTask);
	UE_LOG(LogTemp, Log, TEXT("UThirdPersonMarbleCenteredTask::EnterState"));
	PlayerController->FIA_MainAction.AddDynamic(this, &UThirdPersonMarbleCenteredTask::ToThirdPersonMarbleLaunchTask);
	
	PlayerController->FPossess_Updated.Broadcast(
		PlayerController->PossessedMarble);

	PlayerController->bShowMouseCursor = true;

	CameraOffset = PlayerController->GetPawn()->GetActorLocation()
		- PlayerController->PossessedMarble->GetActorLocation();

	bCenteredOnMarble = false;

	// SetViewTarget with blend to spectator pawn
	PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn(), 0.5f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
	
	return EStateTreeRunStatus::Running;
}

// ExitState
void UThirdPersonMarbleCenteredTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	PlayerController->FIA_Move.RemoveDynamic(this, &UThirdPersonMarbleCenteredTask::CameraMovement);
	PlayerController->FIA_Escape.RemoveDynamic(this, &UThirdPersonMarbleCenteredTask::ToThirdPersonFreeCameraTask);
	PlayerController->FIA_Toggle.RemoveDynamic(this, &UThirdPersonMarbleCenteredTask::ToFirstPersonMarbleCenteredTask);
	PlayerController->FIA_MainAction.RemoveDynamic(this, &UThirdPersonMarbleCenteredTask::ToThirdPersonMarbleLaunchTask);
	
	PlayerController->bShowMouseCursor = false;

	Super::ExitState(Context, Transition);
}

// Tick
EStateTreeRunStatus UThirdPersonMarbleCenteredTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (bCenteredOnMarble)
		SyncAndClampCamera();
	else
		InterpolateCenterOnMarble(DeltaTime);
	return EStateTreeRunStatus::Running;
}

// Interpolate to focus on marble upon state enter
void UThirdPersonMarbleCenteredTask::InterpolateCenterOnMarble(float DeltaTime)
{
	// lerp rotation to look at the center of marble, within bCenterDuration
	APawn* SpectatePawn = PlayerController->GetPawn();
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	FVector SpectatePawnLocation = SpectatePawn->GetActorLocation();
	FVector MarbleLocation = PossessedMarble->GetActorLocation();
	
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(SpectatePawnLocation, MarbleLocation);
	FRotator CurrentRotation = SpectatePawn->GetActorRotation();
	FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, DeltaTime / CenterDuration);
	SpectatePawn->SetActorRotation(NewRotation);

	float CurrentDistance = (SpectatePawnLocation - MarbleLocation).Size();
	float TargetDistance = FMath::Clamp(CurrentDistance, MinCamDist, MaxCamDist);
	float NewDistance = FMath::Lerp(CurrentDistance, TargetDistance, DeltaTime / CenterDuration);
	CameraOffset = (SpectatePawnLocation - MarbleLocation).GetSafeNormal() * NewDistance;
	FVector NewLocation = MarbleLocation + CameraOffset;
	SpectatePawn->SetActorLocation(NewLocation);
	
	// set bCenteredOnMarble if done
	if (FMath::IsNearlyEqual(NewRotation.Yaw, TargetRotation.Yaw, 0.1f) &&
		FMath::IsNearlyEqual(NewRotation.Pitch, TargetRotation.Pitch, 0.1f) &&
		FMath::IsNearlyEqual(NewRotation.Roll, TargetRotation.Roll, 0.1f) &&
		FMath::IsNearlyEqual(NewDistance, TargetDistance, 0.1f))
	{
		SpectatePawn->SetActorRotation(TargetRotation);
		SpectatePawn->SetActorLocation(NewLocation);
		bCenteredOnMarble = true;
		UE_LOG(LogTemp, Log, TEXT("Centered on Marble"));
	}
}

// CameraMovement
void UThirdPersonMarbleCenteredTask::CameraMovement(FVector3f Input)
{
	if (RunStatus != EStateTreeRunStatus::Running)
		return;
	
	// Initialize variablesC
	APawn* SpectatePawn = PlayerController->GetPawn();
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	FVector SpectatePawnLocation = SpectatePawn->GetActorLocation();
	FVector MarbleLocation = PossessedMarble->GetActorLocation();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector Front = SpectatePawn->GetActorForwardVector();
	const FVector Right = SpectatePawn->GetActorRightVector();
	const FVector Up = FVector(0, 0, 1);
	FVector UpdatedOffset = SpectatePawnLocation - MarbleLocation;
	
	// Yaw
	UpdatedOffset = UpdatedOffset.RotateAngleAxis(-Input.Y * YawSpeed, Up);
	
	// Pitch
	float CurrentPitch = UpdatedOffset.Rotation().Pitch;
	float NewPitch = CurrentPitch + Input.Z * PitchSpeed;
	NewPitch = FMath::Clamp(NewPitch, 0.0f, 75.0f);
	UpdatedOffset = UpdatedOffset.RotateAngleAxis(NewPitch - CurrentPitch, Right);

	// Zoom
	float Dist_To_Move = DeltaTime * (ZoomVariableSpeed * UpdatedOffset.Size() + ZoomBaseSpeed);
	const FVector Forward = UpdatedOffset.GetSafeNormal();
	UpdatedOffset -= Forward * Input.X * Dist_To_Move;
	if (UpdatedOffset.Size() < MinCamDist)
		UpdatedOffset = Forward * MinCamDist;

	// Update camera position
	CameraOffset = UpdatedOffset;

	SyncAndClampCamera();
}


void UThirdPersonMarbleCenteredTask::SyncAndClampCamera()
{
	// clamp distance
	if (CameraOffset.Size() < MinCamDist)
		CameraOffset = CameraOffset.GetSafeNormal() * MinCamDist;
	else if (CameraOffset.Size() > MaxCamDist)
		CameraOffset = CameraOffset.GetSafeNormal() * MaxCamDist;

	// clamp pitch, rotate if necessary
	float CurrentPitch = CameraOffset.Rotation().Pitch;
	float NewPitch = FMath::Clamp(CurrentPitch, MinPitch, MaxPitch);
	if (CurrentPitch != NewPitch)
		CameraOffset = CameraOffset.RotateAngleAxis(NewPitch - CurrentPitch, CameraOffset.GetSafeNormal());
	
	APawn* SpectatePawn = PlayerController->GetPawn();
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	FVector MarbleLocation = PossessedMarble->GetActorLocation();

	// Positioning
	SpectatePawn->SetActorLocation(MarbleLocation + CameraOffset);
	// Look at marble
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CameraOffset, FVector::ZeroVector);
	SpectatePawn->SetActorRotation(TargetRotation);
	
}



void UThirdPersonMarbleCenteredTask::ToThirdPersonFreeCameraTask(bool bInspect)
{
	PlayerController->SendStateTreeEventByTagString("Camera.FreeCamera");
}

void UThirdPersonMarbleCenteredTask::ToThirdPersonMarbleLaunchTask(bool bActionPressed)
{
	if (PlayerController->PossessedMarble->IsA<ABaseEnemy>())
	{
		return;
	}
	
	if (bActionPressed and bCenteredOnMarble and PlayerController->PossessedMarble->bReadyToLaunch)
	{
		ATurnBasedGameState* TurnBasedGameState = ATurnBasedGameState::GetInstance();
		if (!TurnBasedGameState) return;
		
		// check if gamestate has a current actor
		if (TurnBasedGameState->CurrentActor)
		{
			ABaseMarble* CurrentMarble = TurnBasedGameState->CurrentActor;
			FString Message = FString::Printf(TEXT("Still %s's turn\nWait for all marbles to stop"),
				*CurrentMarble->AnimalDataAsset->AnimalName);
			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
			AMyGameModeBase* GameMode = AMyGameModeBase::GetInstance();
			UPrimaryHUD* PrimaryHUD = GameMode->PrimaryHUD;
			if (PrimaryHUD)
			{
				PrimaryHUD->ShowInfoBanner(Message, FLinearColor::Yellow, 2.0f);
			}
		}
		else
		{
			PlayerController->SendStateTreeEventByTagString("Action.PrepareLaunch");
		}		
	}
}

void UThirdPersonMarbleCenteredTask::ToFirstPersonMarbleCenteredTask(bool bInspect)
{
	PlayerController->SendStateTreeEventByTagString("Marble.FirstPerson");
}
