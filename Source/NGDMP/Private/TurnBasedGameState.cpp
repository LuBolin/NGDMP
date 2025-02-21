// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameState.h"

#include "MasterPlayerController.h"

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

	// bind key to endturn for debugging
	AMasterPlayerController *MasterPlayerController = AMasterPlayerController::Instance;
	MasterPlayerController->FIA_Debug.AddDynamic(this, &ATurnBasedGameState::EndTurnWrapper);
}

void ATurnBasedGameState::StartTurn()
{
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
	// log size of PlayerMablelsActable
	UE_LOG(LogTemp, Warning, TEXT("PlayerMarblesActable size: %d"), PlayerMarblesActable.Num());
	for (auto& Marble : PlayerMarblesActable)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is dead? %s"), *Marble.Key->GetName(), Marble.Key->bDead ? TEXT("true") : TEXT("false"));
		if (Marble.Key->bDead)
		{
			Marble.Value = false;
		} else
		{
			Marble.Value = true;
			Marble.Key->F_OnStopActing.AddDynamic(this, &ATurnBasedGameState::MarbleEndTurn);
			Marble.Key->GetReadyForNewTurn();	
		}
	}
}

void ATurnBasedGameState::BeginEnemyTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy turn started"));
	for (auto& Enemy : EnemyActorsActable)
	{
		if (Enemy.Key->bDead)
		{
			Enemy.Value = false;
		} else
		{
			Enemy.Value = true;
			Enemy.Key->F_OnStopActing.AddDynamic(this, &ATurnBasedGameState::EnemyActorEndTurn);
			Enemy.Key->GetReadyForNewTurn();	
		}
	}
	EnemyActorStartTurn();
}

void ATurnBasedGameState::EnemyActorStartTurn()
{
	// find an enemy that can act
	// if no enemy can act, end the turn
	for (auto& Enemy : EnemyActorsActable)
	{
		ABaseEnemy* CurrentEnemy = Enemy.Key;
		bool CanAct = Enemy.Value;
		if (CanAct)
		{
			if (CurrentEnemy->bDead)
			{
				Enemy.Value = false;
			} else
			{
				Enemy.Key->Act();
				Enemy.Value = false;
				return;
			}
		}
	}
	EndTurn();
}

void ATurnBasedGameState::EnemyActorEndTurn(ABaseMarble* ActingEnemy)
{
	UE_LOG(LogTemp, Warning, TEXT("%s ended their turn"), *ActingEnemy->GetName());
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

void ATurnBasedGameState::EndTurnWrapper(bool bInput)
{
	for (auto& Enemy : EnemyActorsActable)
	{
		Enemy.Key->CleanUpForEndTurn();
		Enemy.Key->F_OnStopActing.Clear();
	}
	for (auto& Marble : PlayerMarblesActable)
	{
		Marble.Key->CleanUpForEndTurn();
		Marble.Key->F_OnStopActing.Clear();
	}
	EndTurn();
}