// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "TurnBasedGameState.h"

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::Act()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy acting"));
	// emit F_OnStopActing after a 1 second delay
	FTimerHandle StopActingTimerHandle;
	GetWorldTimerManager().SetTimer(StopActingTimerHandle, this, &ABaseEnemy::EndTurn, 1.0f, false);
}

void ABaseEnemy::EndTurn()
{
	Super::EndTurn();
}