// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyStateTreeTaskBlueprintBase.h"
#include "MasterPlayerController.h"
#include "FirstPersonMarbleCenteredTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UFirstPersonMarbleCenteredTask : public UMyStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	constexpr static float SetViewBlendDuration = 0.5f;
	
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
	bool bAnimalHidden = false;
	
	UFUNCTION()
	virtual void CameraPan(FVector2f Input);

	UFUNCTION()
	void ToThirdPersonMarbleCenteredTask(bool bPressed);

	UFUNCTION()
	void UseAbility(bool bPressed);
	
};