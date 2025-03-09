// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameState.h"
#include "MasterPlayerController.h"

ATurnBasedGameState* ATurnBasedGameState::Instance = nullptr;

ATurnBasedGameState::ATurnBasedGameState()
{
	CurrentTurn = ETurnState::PLAYER_TURN;
	PrimaryActorTick.bCanEverTick = true;
	Instance = this;
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
	for (auto& Marble : PlayerMarblesActable)
	{
		if (Marble.Key->bDead)
		{
			Marble.Value = false;
		} else
		{
			Marble.Value = true;
			Marble.Key->FOnStopActing.AddDynamic(this, &ATurnBasedGameState::MarbleEndTurn);
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
			Enemy.Key->FOnStopActing.AddDynamic(this, &ATurnBasedGameState::EnemyActorEndTurn);
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
	ActingEnemy->FOnStopActing.Clear();
	// FTimerHandle EnemyActingTimerHandle;
	// float ArbitraryDelay = 0.5f;
	// GetWorldTimerManager().SetTimer(EnemyActingTimerHandle, this, &ATurnBasedGameState::EnemyActorStartTurn, ArbitraryDelay, false);
}

void ATurnBasedGameState::MarbleEndTurn(ABaseMarble* ActingMarble)
{
	// log who ended their turn
	UE_LOG(LogTemp, Warning, TEXT("%s ended their turn"), *ActingMarble->GetName());
	ActingMarble->FOnStopActing.Clear();
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
		Enemy.Key->FOnStopActing.Clear();
	}
	for (auto& Marble : PlayerMarblesActable)
	{
		Marble.Key->CleanUpForEndTurn();
		Marble.Key->FOnStopActing.Clear();
	}
	EndTurn();
}

void ATurnBasedGameState::CheckMarblesAtRest()
{
	if (not CurrentActor)
	{
		RestDuration = 0.0f;
		return;
	}
	
	// if all marbles are at rest, increment RestDuration by delta
	// else reset the value
	bool AllMarblesAtRest = true;
	// use get keys to retrieve all marbles
	TArray<ABaseMarble*> Marbles;
	PlayerMarblesActable.GetKeys(Marbles);
	for (ABaseMarble* Marble : Marbles)
	{
		if (Marble->GetVelocity().Size() > 0.0f)
		{
			AllMarblesAtRest = false;
			break;
		}
	}
	TArray<ABaseEnemy*> EnemyMarbles;
	EnemyActorsActable.GetKeys(EnemyMarbles);
	for (ABaseEnemy* Enemy : EnemyMarbles)
	{
		if (Enemy->GetVelocity().Size() > 0.0f)
		{
			AllMarblesAtRest = false;
			break;
		}
	}
	if (AllMarblesAtRest)
	{
		RestDuration += GetWorld()->GetDeltaSeconds();
		if (RestDuration > 1.0f)
		{
			CurrentActor = nullptr;
			RestDuration = 0.0f;
			if (CurrentTurn == ETurnState::ENEMY_TURN)
			{
				FTimerHandle EnemyActingTimerHandle;
				float ArbitraryDelay = 0.5f;
				GetWorldTimerManager().SetTimer(EnemyActingTimerHandle, this, &ATurnBasedGameState::EnemyActorStartTurn, ArbitraryDelay, false);
			}
		}
	} else
	{
		RestDuration = 0.0f;
	}
}


void ATurnBasedGameState::Tick(float DeltaTime)
{
	CheckMarblesAtRest();
}