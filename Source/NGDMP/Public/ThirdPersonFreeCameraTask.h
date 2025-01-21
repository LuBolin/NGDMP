// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "MasterPlayerController.h"
#include "MyStateTreeTaskBlueprintBase.h"
#include "ThirdPersonFreeCameraTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UThirdPersonFreeCameraTask : public UMyStateTreeTaskBlueprintBase
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
	ABaseMarble* AimedMarble = nullptr;
	
	UPROPERTY()
	float AimCastLength = 3000.0f;
	
	UPROPERTY()
	float CameraSpeed = 1.0f;

	
	UFUNCTION()
	virtual void CameraMovement(FVector3f Input);

	UFUNCTION()
	virtual void CameraPan(FVector2f Input);

	UFUNCTION()
	void PossessAimedPawn(bool bInspect);
	
	UFUNCTION()
	void ToThirdPersonMarbleCenteredTask(bool bInspect);
};