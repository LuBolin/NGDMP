// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMarbleLaunchTask.h"
#include "MasterPlayerController.h"
#include "StateTreeExecutionContext.h"

#include "DrawDebugHelpers.h"
#include "FirstPersonMarbleCenteredTask.h"
#include "Blueprint/WidgetLayoutLibrary.h"


EStateTreeRunStatus UThirdPersonMarbleLaunchTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	// EnterState of UMyStateTreeTaskBlueprintBase
	Super::Super::EnterState(Context, Transition);

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
	
	// ExitState of UMyStateTreeTaskBlueprintBase
	Super::Super::ExitState(Context, Transition);
}


EStateTreeRunStatus UThirdPersonMarbleLaunchTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	ComputeMouseWorldProjection();
	RenderLaunchMaxCircle();
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
	if (not bMouseProjectionIsValid)
		return;
	
	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	float launchMaxLength = PossessedMarble->AnimalDataAsset->MaxLaunchForce;
	launchMaxLength /= ForceMultiplier;
	FVector Down = FVector(0.0f, 0.0f, -1.0f);
	FVector MarbleBottom = PossessedMarble->GetActorLocation()
		+ Down * PossessedMarble->Radius;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector MouseDirection = MouseWorldProjection - MarbleBottom;
	// clamp MouseDirection to launchMaxLength
	MouseDirection = MouseDirection.GetClampedToMaxSize(launchMaxLength);
	MouseWorldProjection = MouseDirection + MarbleBottom;
	
	DrawDebugLine(GetWorld(), MarbleBottom, MouseWorldProjection,
		FColor::Red, false, DeltaTime, 1, 4.0f);
}

void UThirdPersonMarbleLaunchTask::RenderLaunchMaxCircle()
{
	if (not bMouseProjectionIsValid)
		return;

	ABaseMarble* PossessedMarble = PlayerController->PossessedMarble;
	float launchMaxLength = PossessedMarble->AnimalDataAsset->MaxLaunchForce;
	launchMaxLength /= ForceMultiplier;
	UE_LOG(LogTemp, Warning, TEXT("Launch Max Length: %f"), launchMaxLength);
	// draw a circle around the marble at the launchMaxLength distance
	// project the circle onto the plane of the marble
	FVector Down = FVector(0.0f, 0.0f, -1.0f);
	FVector MarbleBottom = PossessedMarble->GetActorLocation()
		+ Down * PossessedMarble->Radius;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	// use YAxis and ZAxis to adjust the draw plane
	// the circle will be drawn on the plane of the marble
	FVector MouseDirection = MouseWorldProjection - MarbleBottom;
	FVector CircleY = MouseDirection.GetSafeNormal();
	FVector CircleZ = FVector::CrossProduct(CircleY, Down).GetSafeNormal();
	// get 2 vectors that represent this plane that the 
	DrawDebugCircle(GetWorld(), MarbleBottom, launchMaxLength, 32,
		FColor::Green, false, DeltaTime, 0, 4.0f,
		CircleY, CircleZ, true);
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
		Force = FMath::Clamp(Force, 0.0f, PossessedMarble->AnimalDataAsset->MaxLaunchForce);
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