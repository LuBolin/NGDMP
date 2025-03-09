// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

AMyGameModeBase* AMyGameModeBase::Instance = nullptr;
FString AMyGameModeBase::ObjectiveCompleteMessage = "Objective Complete!";

AMyGameModeBase ::AMyGameModeBase() 
{
	PrimaryActorTick.bCanEverTick = true;
	Instance = this;
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMyGameModeBase::WinGame()
{
	OnGameEnd.Broadcast(true);
}

void AMyGameModeBase::LoseGame()
{
	OnGameEnd.Broadcast(false);
}

void AMyGameModeBase::Tick(float DeltaTime)
{
	if (checkDestroyAllEnemiesProgress() == ObjectiveCompleteMessage
		and checkCollectAllStarsProgress() == ObjectiveCompleteMessage)
	{
		WinGame();
	} else {
		bool AnyAlive = false;
		for (ABaseMarble *Marble : PlayerMarbles)
		{
			if (!Marble->bDead)
			{
				AnyAlive = true;
				break;
			}
		}
		if (!AnyAlive)
		{
			LoseGame();
		}
	}
}

FString AMyGameModeBase::checkDestroyAllEnemiesProgress()
{
	int EnemyCount = EnemyActors.Num();
	int AliveEnemyCount = 0;
	for (ABaseEnemy *Enemy : EnemyActors)
	{
		if (!Enemy->bDead)
			AliveEnemyCount++;
	}
	
	FString output = "";
	if (AliveEnemyCount == 0)
		output = ObjectiveCompleteMessage;
	else
		output = FString::FromInt(AliveEnemyCount) + " / " + FString::FromInt(EnemyCount) + " remaining";
	return output;
}

FString AMyGameModeBase::checkCollectAllStarsProgress()
{
	int PickupCount = PickupObjectives.Num();
	if (PickupCount == 0) // in case we checked collectAllStars, but there are no pickups
	{
		collectAllStars = false;
		return ObjectiveCompleteMessage;
	}
	int CollectedPickupCount = 0;
	for (APickupActor *Pickup : PickupObjectives)
	{
		if (Pickup->bCollected)
			CollectedPickupCount++;
	}
	
	FString output = "";
	if (CollectedPickupCount == PickupCount)
		output = ObjectiveCompleteMessage;
	else
		output = FString::FromInt(CollectedPickupCount) + " / " + FString::FromInt(PickupCount) + " collected";
	return output;
}