// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMarbleLaunchTask.h"
#include "MasterPlayerController.h"
#include "StateTreeExecutionContext.h"

#include "DrawDebugHelpers.h"
#include "FirstPersonMarbleCenteredTask.h"
#include "Blueprint/WidgetLayoutLibrary.h"


EStateTreeRunStatus UThirdPersonMarbleLaunchTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("Entering ThirdPersonMarbleLaunchTask"));
	
	if (not PlayerController)
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());

	PlayerController->FIA_Move.AddDynamic(this, &UThirdPersonMarbleLaunchTask::CameraMovement);
	PlayerController->FIA_MainAction.AddDynamic(this, &UThirdPersonMarbleLaunchTask::TryLaunch);
	PlayerController->FIA_Escape.AddDynamic(this, &UThirdPersonMarbleLaunchTask::CancelLaunch);
	
	PlayerController->bShowMouseCursor = true;
	
	CameraOffset = PlayerController->GetPawn()->GetActorLocation()
		- PlayerController->PossessedMarble->GetActorLocation();

	// The value is false, due to inheritance from UThirdPersonMarbleCenteredTask
	// However, entering launch state means this is true already
	// Set this to true for CameraMovement function to work
	bCenteredOnMarble = true;
	
	return EStateTreeRunStatus::Running;
}

void UThirdPersonMarbleLaunchTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	PlayerController->FIA_Move.RemoveDynamic(this, &UThirdPersonMarbleLaunchTask::CameraMovement);
	PlayerController->FIA_MainAction.RemoveDynamic(this, &UThirdPersonMarbleLaunchTask::TryLaunch);
	PlayerController->FIA_Escape.RemoveDynamic(this, &UThirdPersonMarbleLaunchTask::CancelLaunch);
	
	PlayerController->bShowMouseCursor = false;
}


EStateTreeRunStatus UThirdPersonMarbleLaunchTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	ComputeMouseWorldProjection();
	RenderControlLine();
	
	return EStateTreeRunStatus::Running;
}

void UThirdPersonMarbleLaunchTask::ComputeMouseWorldProjection()
{
	FVector2d CurrentMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	CurrentMousePos *= DPIScale;

	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	FVector Normal = PossessedMarble->GetPlaneNormal();
	FVector Down = FVector(0.0f, 0.0f, -1.0f);
	FVector MarbleBottom = PossessedMarble->GetActorLocation() + Down * PossessedMarble->Radius;

	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(
		CurrentMousePos.X, CurrentMousePos.Y, WorldLocation, WorldDirection);
	FVector IntersectionPoint;
	FPlane BottomPlane = FPlane(MarbleBottom, Normal);
	bool Intersects = FMath::SegmentPlaneIntersection(
		WorldLocation, WorldLocation + WorldDirection * 10000.0f,
		BottomPlane, IntersectionPoint);

	bMouseProjectionIsValid = Intersects;
	if (bMouseProjectionIsValid)
	{
		MouseWorldProjection = IntersectionPoint;
	}
}

void UThirdPersonMarbleLaunchTask::RenderControlLine()
{
	if (bMouseProjectionIsValid)
	{
		ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
		FVector Down = FVector(0.0f, 0.0f, -1.0f);
		FVector MarbleBottom = PossessedMarble->GetActorLocation()
			+ Down * PossessedMarble->Radius;
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		DrawDebugLine(GetWorld(), MarbleBottom, MouseWorldProjection,
			FColor::Red, false, DeltaTime, 0, 2.0f);
	}
}


void UThirdPersonMarbleLaunchTask::TryLaunch(bool bLaunchPressed)
{
	if (not bLaunchPressed) // released
	{
		ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
		FVector Down = FVector(0.0f, 0.0f, -1.0f);
		FVector MarbleBottom = PossessedMarble->GetActorLocation()
			+ Down * PossessedMarble->Radius;
		FVector Direction = MarbleBottom - MouseWorldProjection;
		Direction.Normalize();
		float Force = FVector::Dist(MarbleBottom, MouseWorldProjection);
		Force *= ForceMultiplier;
		UE_LOG(LogTemp, Warning, TEXT("Launched with force %f"), Force);
		PlayerController->SendStateTreeEventByTagString("Action.Launched");
		float BlendDelay = UFirstPersonMarbleCenteredTask::SetViewBlendDuration;
		PossessedMarble->Launch(Direction, Force, BlendDelay);
	}
}

void UThirdPersonMarbleLaunchTask::CancelLaunch(bool bEscapePressed)
{
	PlayerController->SendStateTreeEventByTagString("Marble.ThirdPerson");
}