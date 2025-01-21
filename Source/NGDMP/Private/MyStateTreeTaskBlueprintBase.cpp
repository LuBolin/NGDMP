// Fill out your copyright notice in the Description page of Project Settings.

#include "MyStateTreeTaskBlueprintBase.h"

EStateTreeRunStatus UMyStateTreeTaskBlueprintBase::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (not PlayerController)
		PlayerController = Cast<AMasterPlayerController>(Context.GetOwner());

	StateName = GetName();
	
	if (StateName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("StateName is not set"));
		AMasterPlayerController::ExitGame();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Entering %s"), *StateName);
	StateName = StateName.Left(StateName.Find("_"));
	PlayerController->FState_Updated.Broadcast(StateName);
		
	return EStateTreeRunStatus::Succeeded;
}

void UMyStateTreeTaskBlueprintBase::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("Exiting %s"), *StateName);
}