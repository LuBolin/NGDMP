// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "MasterPlayerController.h"
#include "BaseMarble.h"
#include "StateTreeExecutionTypes.h"
#include "SpectateTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API USpectateTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	
protected:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void CameraMovement(FVector3f Input);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void CameraPan(FVector2f Input);

	UFUNCTION(BlueprintCallable)
	void Inspect(bool bInspect);

	
	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	
	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) override;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marble")
	class ABaseMarble* AimedMarble = nullptr;
	

private:
	UPROPERTY()
	class AMasterPlayerController* PlayerController = nullptr;

	float AimCastLength = 3000.0f;
	
	float CameraSpeed = 1.0f;
};