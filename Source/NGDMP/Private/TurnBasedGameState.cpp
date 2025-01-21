// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameState.h"

ATurnBasedGameState::ATurnBasedGameState()
{
	CurrentTurn = ETurnState::PLAYER_TURN;
	CurrentActingEnemy = nullptr;
}

// begin play
void ATurnBasedGameState::BeginPlay()
{
	Super::BeginPlay();
	CurrentTurn = ETurnState::PLAYER_TURN;
	StartTurn();
}

void ATurnBasedGameState::StartTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerMarblesActable: %d"), PlayerMarblesActable.Num());
	UE_LOG(LogTemp, Warning, TEXT("EnemyActorsActable: %d"), EnemyActorsActable.Num());
	UE_LOG(LogTemp, Warning, TEXT("Turn started"));
	// broadcast F_TurnStarted
	if (CurrentTurn == ETurnState::PLAYER_TURN)
		BeginPlayerTurn();
	else
		BeginEnemyTurn();
	F_TurnStarted.Broadcast(CurrentTurn);
}

void ATurnBasedGameState::EndTurn()
{
	CurrentTurn = CurrentTurn == ETurnState::PLAYER_TURN ? ETurnState::ENEMY_TURN : ETurnState::PLAYER_TURN;
	StartTurn();
}

void ATurnBasedGameState::BeginPlayerTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Player turn started"));
	for (auto& Marble : PlayerMarblesActable)
	{
		Marble.Value = true;
		Marble.Key->F_OnStopActing.AddDynamic(this, &ATurnBasedGameState::MarbleEndTurn);
	}
}

void ATurnBasedGameState::BeginEnemyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy turn started"));
	for (auto& Enemy : EnemyActorsActable)
	{
		Enemy.Value = true;
	}
	EnemyActorStartTurn();
}

void ATurnBasedGameState::EnemyActorStartTurn()
{
	// find an enemy that can act
	// if no enemy can act, end the turn
	for (auto& Enemy : EnemyActorsActable)
	{
		if (Enemy.Value)
		{
			Enemy.Key->Act();
			Enemy.Value = false;
			Enemy.Key->F_OnStopActing.AddDynamic(this, &ATurnBasedGameState::EnemyActorEndTurn);
			return;
		}
	}
	EndTurn();
}

void ATurnBasedGameState::EnemyActorEndTurn(ABaseMarble* ActingEnemy)
{
	ActingEnemy->F_OnStopActing.Clear();
	FTimerHandle EnemyActingTimerHandle;
	float ArbitraryDelay = 0.5f;
	GetWorldTimerManager().SetTimer(EnemyActingTimerHandle, this, &ATurnBasedGameState::EnemyActorStartTurn, ArbitraryDelay, false);
}

void ATurnBasedGameState::MarbleEndTurn(ABaseMarble* ActingMarble)
{
	// log who ended their turn
	UE_LOG(LogTemp, Warning, TEXT("%s ended their turn"), *ActingMarble->GetName());
	ActingMarble->F_OnStopActing.Clear();
	PlayerMarblesActable[ActingMarble] = false;
	bool PlayerCanStillAct = false;
	for (auto& Marble : PlayerMarblesActable)
	{
		if (Marble.Value)
		{
			PlayerCanStillAct = true;
			break;
		}
	}
	if (!PlayerCanStillAct)
		EndTurn();
}