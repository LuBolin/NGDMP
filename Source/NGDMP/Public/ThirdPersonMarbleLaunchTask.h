// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterPlayerController.h"
#include "ThirdPersonMarbleCenteredTask.h"
#include "ThirdPersonMarbleLaunchTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UThirdPersonMarbleLaunchTask : public UThirdPersonMarbleCenteredTask
{
	GENERATED_BODY()
	
protected:
	virtual EStateTreeRunStatus EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) override;
	
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	
	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) override;

private:
	UPROPERTY()
	bool bMouseProjectionIsValid = false;
	
	UPROPERTY()
	FVector MouseWorldProjection = FVector::ZeroVector;

	UPROPERTY()
	float ForceMultiplier = 3.0f;
	
	UFUNCTION()
	void ComputeMouseWorldProjection();
	
	UFUNCTION()
	void RenderControlLine();

	UFUNCTION()
	void RenderLaunchMaxCircle();
	
	UFUNCTION()
	void TryLaunch(bool bLaunchReleased);
	
	UFUNCTION()
	void CancelLaunch(bool bEscapeReleased);

	UFUNCTION()
	void PauseCancelLaunchWrapper(bool bIsPaused);

	virtual void SetupMousecaptureAndFocus(bool bIsPaused) override;
};