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
	NeutralOutlineColor = FLinearColor::Red;
	AimedOutlineColor = FLinearColor(0.4f, 0.0f, 0.0f, 1.0f);
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
	
void ABaseEnemy::Act()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy acting"));
	// choose a random direction on its normal
	// move in that direction
	FVector Normal = GetPlaneNormal();
	UE_LOG(LogTemp, Warning, TEXT("Normal: %s"), *Normal.ToString());
	FVector RandomDirection = GetRandomVectorOnPlane(Normal);
	float Force = 400.0f;
	float BlendDelay = 0.3f;
	Launch(RandomDirection, Force, BlendDelay);
	UE_LOG(LogTemp, Warning, TEXT("Enemy acted"));
	
	// emit F_OnStopActing after a 1 second delay
	// FTimerHandle StopActingTimerHandle;
	// GetWorldTimerManager().SetTimer(StopActingTimerHandle, this, &ABaseEnemy::EndTurn, 1.0f, false);
}

void ABaseEnemy::EndTurn()
{
	Super::EndTurn();
}
