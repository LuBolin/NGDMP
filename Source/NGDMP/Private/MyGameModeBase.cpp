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
	// Implement this function
}

void AMyGameModeBase::LoseGame()
{
	// Implement this function
}

void AMyGameModeBase::RestartGame()
{
	// Implement this function
}

void AMyGameModeBase::Tick(float DeltaTime)
{
	
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