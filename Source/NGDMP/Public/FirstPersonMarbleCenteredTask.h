// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "MasterPlayerController.h"
#include "FirstPersonMarbleCenteredTask.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UFirstPersonMarbleCenteredTask : public UStateTreeTaskBlueprintBase
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
	
	UPROPERTY()
	AMasterPlayerController* PlayerController = nullptr;

	UFUNCTION()
	void Ability1(bool bClicked);
	
	UFUNCTION()
	virtual void CameraPan(FVector2f Input);

	UFUNCTION()
	void ToThirdPersonMarbleCenteredTask(bool bPressed);
	
};