// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "BaseEnemy.h"
#include "GameFramework/GameStateBase.h"
#include "TurnBasedGameState.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ETurnState : uint8
{
	PLAYER_TURN UMETA(DisplayName = "Player's Turn"),
	ENEMY_TURN UMETA(DisplayName = "Enemy's Turn")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F_TurnStarted, ETurnState, NewState);

UCLASS()
class NGDMP_API ATurnBasedGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATurnBasedGameState();

	virtual void BeginPlay() override;
	
	// Enum to define turn states
	UPROPERTY(BlueprintReadOnly, Category = "Turn System")
	ETurnState CurrentTurn;

	// Functions to start and end turns
	UFUNCTION(BlueprintCallable, Category = "Turn System")
	void StartTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn System")
	void EndTurn();
	
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	F_TurnStarted F_TurnStarted;

	UPROPERTY()
	TMap<class ABaseMarble*, bool> PlayerMarblesActable;
	
	UPROPERTY()
	TMap<class ABaseEnemy*, bool> EnemyActorsActable;

	UPROPERTY()
	TArray<class APickupActor*> PickupObjectives;
	
	UPROPERTY()
	ABaseMarble* CurrentActor = nullptr;
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	static ATurnBasedGameState* GetInstance()
	{
		return Instance;
	}

	static ATurnBasedGameState* Instance;
	
private:
	UFUNCTION()
	void BeginPlayerTurn();
	
	UFUNCTION()
	void BeginEnemyTurn();
	
	UFUNCTION()
	void EnemyActorStartTurn();
	
	UFUNCTION()
	void CurrentEnemyActorEndTurn();

	UFUNCTION()
	void CurrentPlayerMarbleEndTurn();

	UFUNCTION()
	void EndTurnWrapper(bool bInput);

	UFUNCTION()
	void CheckMarblesAtRest();

	UPROPERTY()
	float RestDuration = 0.0f;

	UPROPERTY()
	float RestGoal = 0.0f;

	
};
