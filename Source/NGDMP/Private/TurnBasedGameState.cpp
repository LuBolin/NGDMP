// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnBasedGameState.h"
#include "MasterPlayerController.h"
#include "MyGameModeBase.h"
#include "PrimaryHUD.h"

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

	// bind key to endturn for debugging
	AMasterPlayerController *MasterPlayerController = AMasterPlayerController::Instance;
	MasterPlayerController->FIA_Debug.AddDynamic(this, &ATurnBasedGameState::EndTurnWrapper);

	// wait for 0.2 seconds before starting the turn
	FTimerHandle TurnStartTimerHandle;
	float ArbitraryDelay = 0.2f;
	GetWorldTimerManager().SetTimer(TurnStartTimerHandle,
		this, &ATurnBasedGameState::StartTurn, ArbitraryDelay, false);
}

void ATurnBasedGameState::StartTurn()
{
	F_TurnStarted.Broadcast(CurrentTurn);

	// when delay finish, then start the turn
	float turnInfoBannerDuration = UPrimaryHUD::turnTransitionDuration;
	FTimerHandle TurnStartTimerHandle;

	auto SpecificStartTurnMethod = (CurrentTurn == ETurnState::PLAYER_TURN)
		? &ATurnBasedGameState::BeginPlayerTurn : &ATurnBasedGameState::BeginEnemyTurn;
	GetWorldTimerManager().SetTimer(TurnStartTimerHandle, this, SpecificStartTurnMethod, turnInfoBannerDuration, false);
	
}

void ATurnBasedGameState::EndTurn()
{
	CurrentTurn = CurrentTurn == ETurnState::PLAYER_TURN ? ETurnState::ENEMY_TURN : ETurnState::PLAYER_TURN;
	StartTurn();
}

void ATurnBasedGameState::BeginPlayerTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Player turn started"));
	ABaseMarble* ActableMarble = nullptr;
	FVector CameraLocation = AMasterPlayerController::Instance->GetPawn()->GetActorLocation();
	for (auto& MarbleTuple : PlayerMarblesActable)
	{
		ABaseMarble* Marble = MarbleTuple.Key;
		
		if (Marble->bDead)
		{
			MarbleTuple.Value = false;
		} else
		{
			MarbleTuple.Value = true;

			if (not ActableMarble)
				ActableMarble = Marble;
			else
			{
				// distance from actable marble to camera
				float actableDist = (ActableMarble->GetActorLocation() - CameraLocation).Size();
				float marbleDist = (Marble->GetActorLocation() - CameraLocation).Size();
				if (marbleDist < actableDist)
					ActableMarble = Marble;
			}
			
			
			Marble->GetReadyForNewTurn();
		}
	}
	
	ABaseMarble* CurrentPossessedMarble = AMasterPlayerController::Instance->PossessedMarble;
	bool PossessingActableMarble = CurrentPossessedMarble and not CurrentPossessedMarble->bReadyToLaunch; 
	if (ActableMarble and not PossessingActableMarble)
	{
		AMasterPlayerController *PlayerController = AMasterPlayerController::Instance;
		PlayerController->ForceFocusOnMarble(ActableMarble);
	} else
	{
		EndTurn();
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
			Enemy.Key->GetReadyForNewTurn();	
		}
	}
	EnemyActorStartTurn();
}

void ATurnBasedGameState::EnemyActorStartTurn()
{
	// find an enemy that can act
	// if no enemy can act, end the turn
	for (auto& EnemyTuple : EnemyActorsActable)
	{
		ABaseEnemy* Enemy = EnemyTuple.Key;
		bool CanAct = EnemyTuple.Value;
		if (CanAct)
		{
			if (Enemy->bDead)
			{
				EnemyTuple.Value = false;
			} else
			{
				// look at marble that is going to act
				AMasterPlayerController *PlayerController = AMasterPlayerController::Instance;
				PlayerController->ForceFocusOnMarble(Enemy);
				
				Enemy->Act();
				EnemyTuple.Value = false;
				return;
			}
		}
	}
	EndTurn();
}

void ATurnBasedGameState::CurrentEnemyActorEndTurn()
{
	FTimerHandle EnemyActingTimerHandle;
	float ArbitraryDelay = 0.5f;
	GetWorldTimerManager().SetTimer(EnemyActingTimerHandle, this, &ATurnBasedGameState::EnemyActorStartTurn, ArbitraryDelay, false);
}

void ATurnBasedGameState::CurrentPlayerMarbleEndTurn()
{
	// log who ended their turn
	UE_LOG(LogTemp, Warning, TEXT("%s ended their turn"), *CurrentActor->GetName());
	PlayerMarblesActable[CurrentActor] = false;
	ABaseMarble* ActableMarble = nullptr;
	FVector CameraLocation = AMasterPlayerController::Instance->GetPawn()->GetActorLocation();

	// if all enemy are dead, let all marbles act all the time
	AMyGameModeBase* GameMode = AMyGameModeBase::GetInstance();
	bool AllEnemyDead = GameMode->checkDestroyAllEnemiesProgress() == GameMode->ObjectiveCompleteMessage;
	if (AllEnemyDead)
    {
        for (auto& MarbleTuple : PlayerMarblesActable)
        {
        	MarbleTuple.Value = true;
        	MarbleTuple.Key->GetReadyForNewTurn();
        }
    }

	
	for (auto& MarbleTuple : PlayerMarblesActable)
	{
		ABaseMarble* Marble = MarbleTuple.Key;
		bool CanAct = MarbleTuple.Value;

		if (not CanAct)
			continue;
		
		if (not ActableMarble)
			ActableMarble = Marble;
		else
		{
			float actableDist = (ActableMarble->GetActorLocation() - CameraLocation).Size();
			float marbleDist = (Marble->GetActorLocation() - CameraLocation).Size();
			if (marbleDist < actableDist)
				ActableMarble = Marble;
		}
	}
	
	ABaseMarble* CurrentPossessedMarble = AMasterPlayerController::Instance->PossessedMarble;
	bool PossessingActableMarble = CurrentPossessedMarble and not CurrentPossessedMarble->bReadyToLaunch;
	if (ActableMarble and not PossessingActableMarble)
	{
		AMasterPlayerController *PlayerController = AMasterPlayerController::Instance;
		PlayerController->ForceFocusOnMarble(ActableMarble);
	} else
	{
		EndTurn();
	}
	
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
			if (CurrentTurn == ETurnState::ENEMY_TURN)
			{
				CurrentEnemyActorEndTurn();
			}
			else
			{
				CurrentPlayerMarbleEndTurn();
			}
			
			CurrentActor = nullptr;
			RestDuration = 0.0f;
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