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
	ATurnBasedGameState* TurnBasedGameState = ATurnBasedGameState::GetInstance();
	if (!TurnBasedGameState) return;

	TurnBasedGameState->CurrentActor = this;
	
	FVector LaunchDirection = FVector::ZeroVector;
	
	TArray<ABaseMarble*> PlayerMarbles;
	TurnBasedGameState->PlayerMarblesActable.GetKeys(PlayerMarbles);
	
	constexpr float ConfidenceDistance = 1000.0f;
	// remove all marbles in PlayerMarbles that are dead or out of range
	for (int i = 0; i < PlayerMarbles.Num(); i++)
	{
		float Distance = FVector::Dist(PlayerMarbles[i]->GetActorLocation(), GetActorLocation());
		if (PlayerMarbles[i]->bDead or Distance > ConfidenceDistance)
		{
			PlayerMarbles.RemoveAt(i);
			i--;
		}
	}
	// sort PlayerMarbles by distance
	PlayerMarbles.Sort([this](ABaseMarble& A, ABaseMarble& B) {
		FVector ALocation = A.GetActorLocation();
		FVector BLocation = B.GetActorLocation();
		FVector OwnLocation = GetActorLocation();
		float ADistance = FVector::Dist(ALocation, OwnLocation);
		float BDistance = FVector::Dist(BLocation, OwnLocation);
		return ADistance < BDistance;
	});
	
	bool HasVisiblePlayerInRange = false;
	for (ABaseMarble *Marble : PlayerMarbles)
	{
		FVector PlayerMarbleLocation = Marble->GetActorLocation();
		FVector OwnLocation = GetActorLocation();
		float Distance = FVector::Dist(PlayerMarbleLocation, OwnLocation);
		float ConfidentToHitDist = 1000.0f;
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
				// UE_LOG(LogTemp, Warning, TEXT("Launch direction: %s"), *LaunchDirection.ToString());
				// FVector debug = (PlayerMarbleLocation - OwnLocation);
				// debug.Normalize();
				// UE_LOG(LogTemp, Warning, TEXT("Normalized direction: %s"), *debug.ToString());
				HasVisiblePlayerInRange = true;
				break;
			} else {
				// 50% chance to normalize the direction about its own plane and launch off anyways
				if (FMath::RandRange(0, 1) == 1)
				{
					FVector Normal = GetPlaneNormal();
					LaunchDirection = (PlayerMarbleLocation - OwnLocation).GetSafeNormal();
					// project launch_direction onto the normal plane
					LaunchDirection = LaunchDirection - FVector::DotProduct(LaunchDirection, Normal) * Normal;
					HasVisiblePlayerInRange = true;
					break;
				}
			}
		}
	}
	if (not HasVisiblePlayerInRange)
	{
		FVector Normal = GetPlaneNormal();
		// 50% chance to move randomly
		// 50% chance to "be dumb" and move towards the nearest marble anyways
		// check if playermarbles is empty
		if (FMath::RandRange(0, 1) == 1 or PlayerMarbles.Num() == 0)
		{
			LaunchDirection = GetRandomVectorOnPlane(Normal);
		} else
		{
			ABaseMarble* Marble = PlayerMarbles[0];
			FVector PlayerMarbleLocation = Marble->GetActorLocation();
			FVector OwnLocation = GetActorLocation();
			LaunchDirection = (PlayerMarbleLocation - OwnLocation).GetSafeNormal();
			LaunchDirection = LaunchDirection - FVector::DotProduct(LaunchDirection, Normal) * Normal;
		}
	}
	
	LaunchDirection.Normalize();
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
