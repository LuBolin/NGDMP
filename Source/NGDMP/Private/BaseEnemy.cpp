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
	// AimedOutlineColor = FLinearColor::Red;
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
	
void ABaseEnemy::Act()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy acting"));

	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is not valid"));
		return;
	}
	
	FVector LaunchDirection = FVector::ZeroVector;
	
	TArray<ABaseMarble*> PlayerMarbles;
	TurnBasedGameState->PlayerMarblesActable.GetKeys(PlayerMarbles);
	bool HasPlayerInRange = false;
	for (ABaseMarble *Marble : PlayerMarbles)
	{
		FVector PlayerMarbleLocation = Marble->GetActorLocation();
		FVector OwnLocation = GetActorLocation();
		float Distance = FVector::Dist(PlayerMarbleLocation, OwnLocation);
		float ConfidentToHitDist = 500.0f;
		UE_LOG(LogTemp, Warning, TEXT("Distance with %s: %f"), *Marble->GetName(), Distance);
		if (Distance < ConfidentToHitDist)
		{
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			CollisionParams.AddIgnoredActor(Marble);
			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult, OwnLocation, PlayerMarbleLocation, ECollisionChannel::ECC_Visibility, CollisionParams);

			if (not bHit) // clear hit
			{
				LaunchDirection = (PlayerMarbleLocation - OwnLocation).GetSafeNormal();
				UE_LOG(LogTemp, Warning, TEXT("Launch direction: %s"), *LaunchDirection.ToString());
				FVector debug = (PlayerMarbleLocation - OwnLocation);
				debug.Normalize();
				UE_LOG(LogTemp, Warning, TEXT("Normalized direction: %s"), *debug.ToString());
				HasPlayerInRange = true;
				break;
			}
		}
	}
	if (not HasPlayerInRange)
	{
		FVector Normal = GetPlaneNormal();
		// choose a random direction on its normal
		// move in that direction
		LaunchDirection = GetRandomVectorOnPlane(Normal);
	}
	
	float Force = 600.0f;
	float BlendDelay = 0.3f; // this cannot be 0
	Launch(LaunchDirection, Force, BlendDelay);
	UE_LOG(LogTemp, Warning, TEXT("Enemy acted"));
	
	// emit F_OnStopActing after a 1 second delay
	// FTimerHandle StopActingTimerHandle;
	// GetWorldTimerManager().SetTimer(StopActingTimerHandle, this, &ABaseEnemy::EndTurn, 1.0f, false);
}

void ABaseEnemy::EndTurn()
{
	Super::EndTurn();
}
