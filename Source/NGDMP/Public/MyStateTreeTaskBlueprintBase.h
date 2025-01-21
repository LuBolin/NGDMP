// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MasterPlayerController.h"
#include "StateTreeExecutionContext.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "CoreMinimal.h"
#include "MyStateTreeTaskBlueprintBase.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UMyStateTreeTaskBlueprintBase : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	FString StateName;
	
	UPROPERTY()
	AMasterPlayerController* PlayerController = nullptr;
	
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	
	// virtual EStateTreeRunStatus Tick(
	// 	FStateTreeExecutionContext& Context,
	// 	const float DeltaTime) override;

};
