// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectTask.h"
#include "MasterPlayerController.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateTreeExecutionContext.h"
#include "Blueprint/WidgetLayoutLibrary.h"

// EnterState
EStateTreeRunStatus UInspectTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (not PlayerController)
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());
	
	PlayerController->FIA_Move.AddDynamic(this, &UInspectTask::CameraMovement);
	PlayerController->FIA_Inspect.AddDynamic(this, &UInspectTask::UnInspect);
	PlayerController->FIA_MainAction.AddDynamic(this, &UInspectTask::SetAiming);
	PlayerController->FIA_Escape.AddDynamic(this, &UInspectTask::UnInspect);
	
	PlayerController->FPossess_Updated.Broadcast(
		PlayerController->PossessedMarble);

	PlayerController->bShowMouseCursor = true;

	CameraOffset = PlayerController->GetPawn()->GetActorLocation()
		- PlayerController->PossessedMarble->GetActorLocation();
	
	return EStateTreeRunStatus::Running;
}

// ExitState
void UInspectTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	PlayerController->FIA_Move.RemoveDynamic(this, &UInspectTask::CameraMovement);
	PlayerController->FIA_Inspect.RemoveDynamic(this, &UInspectTask::UnInspect);
	PlayerController->FIA_MainAction.RemoveDynamic(this, &UInspectTask::SetAiming);
	PlayerController->FIA_Escape.RemoveDynamic(this, &UInspectTask::UnInspect);
	
	PlayerController->bShowMouseCursor = false;
}

// CameraMovement
void UInspectTask::CameraMovement(FVector3f Input)
{
	if (RunStatus != EStateTreeRunStatus::Running)
		return;

	APawn* SpectatePawn = PlayerController->GetPawn();
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	const FVector Front = SpectatePawn->GetActorForwardVector();
	const FVector Right = SpectatePawn->GetActorRightVector();
	const FVector Up = FVector(0, 0, 1);

	FVector UpdatedOffset =
		SpectatePawn->GetActorLocation() - PossessedMarble->GetActorLocation();
	
	// Yaw
	UpdatedOffset = UpdatedOffset.RotateAngleAxis(-Input.Y * YawSpeed, Up);
	
	// Pitch
	float CurrentPitch = UpdatedOffset.Rotation().Pitch;
	float NewPitch = CurrentPitch + Input.Z * PitchSpeed;
	NewPitch = FMath::Clamp(NewPitch, 0.0f, 75.0f);
	UpdatedOffset = UpdatedOffset.RotateAngleAxis(NewPitch - CurrentPitch, Right);

	// Zoom
	float Delta_Time = GetWorld()->GetDeltaSeconds();
	float Dist_To_Move = Delta_Time *
		(ZoomVariableSpeed * UpdatedOffset.Size() + ZoomBaseSpeed);
	const FVector Forward = UpdatedOffset.GetSafeNormal();
	UpdatedOffset -= Forward * Input.X * Dist_To_Move;
	if (UpdatedOffset.Size() < MinCamDist)
		UpdatedOffset = Forward * MinCamDist;

	CameraOffset = UpdatedOffset;
	
	SpectatePawn->SetActorLocation(PossessedMarble->GetActorLocation() + CameraOffset);
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(
		SpectatePawn->GetActorLocation(), PossessedMarble->GetActorLocation());
	SpectatePawn->SetActorRotation(NewRotation);
	
}

// Drag to aim for launch
EStateTreeRunStatus UInspectTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	
	if (Aiming)
	{
		FVector2d CurrentMousePos = UWidgetLayoutLibrary
			::GetMousePositionOnViewport(GetWorld());
		// FVector2d ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
		float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
		CurrentMousePos *= DPIScale;
	
		ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
		FVector Normal = PossessedMarble->GetPlaneNormal();
		FVector Down = FVector(0.0f, 0.0f, -1.0f);
		FVector MarbleBottom = PossessedMarble->GetActorLocation()
			+ Down * PossessedMarble->Radius;

		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectScreenPositionToWorld(
			CurrentMousePos.X, CurrentMousePos.Y, WorldLocation, WorldDirection);
		FVector IntersectionPoint;
		FPlane BottomPlane = FPlane(MarbleBottom, Normal);
		bool Intersects = FMath::SegmentPlaneIntersection(
			WorldLocation, WorldLocation + WorldDirection * 10000.0f,
			BottomPlane, IntersectionPoint);
		if (Intersects)
		{
			DrawDebugLine(GetWorld(), MarbleBottom, IntersectionPoint,
				FColor::Red, false, DeltaTime, 0, 2.0f);
			LaunchSource = IntersectionPoint;
		}
	}


	// Look At Marble
	APawn* SpectatePawn = PlayerController->GetPawn();
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;

	FVector CurrentOffset = SpectatePawn->GetActorLocation()
		- PossessedMarble->GetActorLocation();
	if (CurrentOffset != CameraOffset)
	{
		SpectatePawn->SetActorLocation(PossessedMarble->GetActorLocation() + CameraOffset);
		// Both offset and rotation are set to the same value
	}
	
	return EStateTreeRunStatus::Running;
}


void UInspectTask::SetAiming(bool bAim)
{
	Aiming = bAim;
	if (not Aiming)
	{
		ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
		FVector Down = FVector(0.0f, 0.0f, -1.0f);
		FVector MarbleBottom = PossessedMarble->GetActorLocation()
			+ Down * PossessedMarble->Radius;
		FVector Direction = MarbleBottom - LaunchSource;
		Direction.Normalize();
		float Force = FVector::Dist(MarbleBottom, LaunchSource);
		PossessedMarble->Launch(Direction, Force);
	}
}

void UInspectTask::UnInspect(bool bInspect)
{
	PlayerController->StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(
		FGameplayTag::RequestGameplayTag(FName("Inspect.UnInspect"))));
}