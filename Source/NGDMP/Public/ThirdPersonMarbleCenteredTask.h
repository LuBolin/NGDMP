// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "MasterPlayerController.h"
#include "ThirdPersonMarbleCenteredTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UThirdPersonMarbleCenteredTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	UPROPERTY() 
	AMasterPlayerController* PlayerController = nullptr;

	UPROPERTY()
	FVector CameraOffset = FVector::ZeroVector;
	
	UPROPERTY()
	bool bCenteredOnMarble = false;
	
	UFUNCTION()
	void CameraMovement(FVector3f Input);
	
protected:
	constexpr static float YawSpeed = 1.0f;
	constexpr static float PitchSpeed = 1.0f;
	constexpr static float ZoomBaseSpeed = 5.0f;
	constexpr static float ZoomVariableSpeed = 0.75f;
	constexpr static float MinPitch = 0.0f;
	constexpr static float MaxPitch = 75.0f;
	constexpr static float MinCamDist = 240.0f;
	constexpr static float MaxCamDist = 1000.0f;
	
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
	
	constexpr static float CenterDuration = 0.1f;
	
	UFUNCTION()
	void InterpolateCenterOnMarble(float DeltaTime);
	
	UFUNCTION()
	void ToThirdPersonFreeCameraTask(bool bInspect);
	
	UFUNCTION()
	void ToThirdPersonMarbleLaunchTask(bool bInspect);
	
	UFUNCTION()
	void ToFirstPersonMarbleCenteredTask(bool bInspect);

	UFUNCTION()
	void SyncAndClampCamera();
};