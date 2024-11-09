// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPlayerController.h"
#include "BaseMarble.h"
#include "Kismet/KismetMathLibrary.h"


AMasterPlayerController* AMasterPlayerController::Instance = nullptr;


// on begin play, set the instance to this controller
void AMasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
	CameraPawn = GetPawn();
    SetViewTargetWithBlend(CameraPawn, 0.5f);
}

void AMasterPlayerController::CameraMovement(FVector3f Input)
{
	const FVector Front = CameraPawn->GetActorForwardVector();
	const FVector Right = CameraPawn->GetActorRightVector();
	const FVector Up = FVector(0, 0, 1);
	
	if (bFocusedOnMarble && PossessedMarble != nullptr)
	{
		FVector OffsetDirection =
			CameraPawn->GetActorLocation() - PossessedMarble->GetActorLocation();

		// Yaw
		OffsetDirection = OffsetDirection.RotateAngleAxis(-Input.Y * CameraSpeed, Up);

		// Pitch
		float CurrentPitch = OffsetDirection.Rotation().Pitch;
		float NewPitch = CurrentPitch + Input.Z * CameraSpeed;
		NewPitch = FMath::Clamp(NewPitch, 0.0f, 75.0f);
		OffsetDirection = OffsetDirection.RotateAngleAxis(NewPitch - CurrentPitch, Right);
		
		// Zoom
		// every second, move up to 75% of the distance + 5 (base speed)
		float Delta_Time = GetWorld()->GetDeltaSeconds();
		float Dist_To_Move = Delta_Time * 0.75 * OffsetDirection.Size() + 5;
		const FVector Forward = OffsetDirection.GetSafeNormal();
		OffsetDirection -= Forward * Input.X * Dist_To_Move;
		// closest we can get is within 300 units of the marble
		if (OffsetDirection.Size() < MinCamDist)
		{
			OffsetDirection = Forward * MinCamDist;
		}
		
		CameraPawn->SetActorLocation(PossessedMarble->GetActorLocation() + OffsetDirection);
		
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CameraPawn->GetActorLocation(), PossessedMarble->GetActorLocation());
		CameraPawn->SetActorRotation(NewRotation);
	}
	else {
		FVector Target_Movement = Front * Input.X + Right * Input.Y + Up * Input.Z;
		Target_Movement.Normalize();
		
		CameraPawn->AddMovementInput(Target_Movement, CameraSpeed);
	}
}

void AMasterPlayerController::ToggleFocusOnMarble()
{
	if (PossessedMarble == nullptr)
	{
		bFocusedOnMarble = false;
		return;
	}
	
	bFocusedOnMarble = !bFocusedOnMarble;
	if (bFocusedOnMarble)
	{
		const FRotator New_Rotation = UKismetMathLibrary::FindLookAtRotation(
			CameraPawn->GetActorLocation(), PossessedMarble->GetActorLocation());
		CameraPawn->SetActorRotation(New_Rotation);
		SetControlRotation(New_Rotation);
	}
}

void AMasterPlayerController::CameraPan(FVector2f Input)
{
	if (bFocusedOnMarble && PossessedMarble != nullptr)
	{
		// Nothing. Mouse movement when focused on marble
		// should be for launching the marble
	}
	else { // if not focused on marble
		AddYawInput(Input.X);
		AddPitchInput(Input.Y);
		// set based on self rotation
		CameraPawn->SetActorRotation(GetControlRotation());
	}
}

void AMasterPlayerController::SetMarblePerspective(bool bFirstPerson)
{
	if (PossessedMarble == nullptr)
	{
		FirstPerson = false;
		return;
	}

	FirstPerson = bFirstPerson;
	
	// set view target with blend
	if (FirstPerson)
    {
		StoredCamDist = (CameraPawn->GetActorLocation() - PossessedMarble->GetActorLocation()).Size();
		SetViewTargetWithBlend(PossessedMarble, 0.5f);
    }
    else
    {
    	// target should be StoredCamDist away from marble
    	// moving 'backwards' from the current camera view direction
    	FVector Front = CameraPawn->GetActorForwardVector();
    	Front.Z = 0;
    	const FVector Up = FVector(0, 0, 1);
    	const FVector BackDirn = (Up - Front).GetSafeNormal();
    	const FVector Offset = BackDirn * StoredCamDist;
    	const FVector NewLocation = PossessedMarble->GetActorLocation() + Offset;
    	CameraPawn->SetActorLocation(NewLocation);
    	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CameraPawn->GetActorLocation(), PossessedMarble->GetActorLocation());
    	CameraPawn->SetActorRotation(NewRotation);
    	SetViewTargetWithBlend(CameraPawn, 0.5f);
    }
}

















