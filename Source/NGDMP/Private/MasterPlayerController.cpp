// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPlayerController.h"


AMasterPlayerController* AMasterPlayerController::Instance = nullptr;

AMasterPlayerController::AMasterPlayerController()
{
	Instance = this;
}

// on begin play, set the instance to this controller
void AMasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SpectatePawn = GetPawn();
	StateTreeComponent = FindComponentByClass<UStateTreeComponent>();
	// Manually start logic, so inits such as HUD's binding are set
	UE_LOG(LogTemp, Warning, TEXT("State Tree Component: %s"), StateTreeComponent ? TEXT("Found") : TEXT("Not Found"));
	if (!StateTreeComponent)
	{
		ExitGame();
		return;
	}
	StateTreeComponent->StartLogic();
	
	SetViewTargetWithBlend(SpectatePawn, 0.5f);
}

void AMasterPlayerController::SendStateTreeEventByTagString(FString TagString)
{
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString));
	StateTreeComponent->SendStateTreeEvent(FStateTreeEvent(Tag));
	UE_LOG(LogTemp, Log, TEXT("State Tree Event: %s"), *TagString);
}

void AMasterPlayerController::ExitGame()
{
	Instance->GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void AMasterPlayerController::ForceFocusOnMarble(ABaseMarble* Marble)
{
	UE_LOG(LogTemp, Warning, TEXT("Forcing focus on: %s"), *Marble->GetName());
	// 1 delta ~= 1 frame
	float FrameDelay = GetWorld()->GetDeltaSeconds();
	
	// in case in first person, pop to third person centered first
	// to set camera position
	if (not PossessedMarble)
	{	// PossessedMarble needs to be set for ThirdPersonMarbleCenteredTask to work
		PossessedMarble = Marble;
		FPossess_Updated.Broadcast(Marble);
	}
	SendStateTreeEventByTagString("Marble.ThirdPerson");
	
	// then no matter what state, go to third person free cam first
	// to 'reset' the camera
	FTimerHandle TimerHandle1;
	float delay1 = FrameDelay;
	auto GoToFreeCam = [this]()
	{
		SendStateTreeEventByTagString("Camera.FreeCamera");
	};
	GetWorld()->GetTimerManager().SetTimer(TimerHandle1, GoToFreeCam, delay1, false);
	
	// then go to third person centered on marble
	// but first wait for 1 frame for the previous signal to be processed
	FTimerHandle TimerHandle2;
	float delay2 = 2 * FrameDelay; // have to wait for the previous delay too
	auto GoToThirdPersonCentered = [this, Marble]()
	{
		PossessedMarble = Marble;
		FPossess_Updated.Broadcast(Marble);
		SendStateTreeEventByTagString("Marble.ThirdPerson");
	};
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, GoToThirdPersonCentered, delay2, false);
}