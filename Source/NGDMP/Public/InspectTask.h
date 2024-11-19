// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "MasterPlayerController.h"
#include "BaseMarble.h"
#include "StateTreeExecutionContext.h"
#include "InspectTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UInspectTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	
protected:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void CameraMovement(FVector3f Input);

	UFUNCTION(BlueprintCallable)
	void UnInspect(bool bInspect);
	
	UFUNCTION(BlueprintCallable)
	void SetAiming(bool bAim);


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
	AMasterPlayerController* PlayerController = nullptr;

	float YawSpeed = 1.0f;
	float PitchSpeed = 1.0f;
	float ZoomBaseSpeed = 5.0f;
	float ZoomVariableSpeed = 0.75f;
	float MinCamDist = 320.0f;
	float MaxCamDist = 2000.0f;
	float MinPitch = 0.0f;
	float MaxPitch = 75.0f;
	
	FVector CameraOffset = FVector::ZeroVector;
	
	bool Aiming = false;
	FVector LaunchSource = FVector::ZeroVector;

};