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