// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPlayerController.h"
#include "BaseMarble.h"


AMasterPlayerController* AMasterPlayerController::Instance = nullptr;


// on begin play, set the instance to this controller
void AMasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
	SpectatePawn = GetPawn();
	StateTreeComponent = FindComponentByClass<UStateTreeComponent>();
	
	SetViewTargetWithBlend(SpectatePawn, 0.5f);
}

// void AMasterPlayerController::SetMarblePerspective(bool bFirstPerson)
// {
// 	if (PossessedMarble == nullptr)
// 	{
// 		FirstPerson = false;
// 		return;
// 	}
//  
// 	FirstPerson = bFirstPerson;
// 	
// 	// set view target with blend
// 	if (FirstPerson)
//     {
// 		StoredCamDist = (CameraPawn->GetActorLocation() - PossessedMarble->GetActorLocation()).Size();
// 		SetViewTargetWithBlend(PossessedMarble, 0.5f);
//     }
//     else
//     {
//     	// target should be StoredCamDist away from marble
//     	// moving 'backwards' from the current camera view direction
//     	FVector Front = CameraPawn->GetActorForwardVector();
//     	Front.Z = 0;
//     	const FVector Up = FVector(0, 0, 1);
//     	const FVector BackDirn = (Up - Front).GetSafeNormal();
//     	const FVector Offset = BackDirn * StoredCamDist;
//     	const FVector NewLocation = PossessedMarble->GetActorLocation() + Offset;
//     	CameraPawn->SetActorLocation(NewLocation);
//     	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CameraPawn->GetActorLocation(), PossessedMarble->GetActorLocation());
//     	CameraPawn->SetActorRotation(NewRotation);
//     	SetViewTargetWithBlend(CameraPawn, 0.5f);
//     }
// }
//
















