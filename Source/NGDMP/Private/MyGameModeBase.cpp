// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

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

	if (StartSound)
	{
		FVector Location = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StartSound, Location);
	}

	if (BackgroundMusic)
	{
		UAudioComponent* BackgroundMusicComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BackgroundMusic);
		float FadeInDurn = 2.0f;
		BackgroundMusicComponent->FadeIn(FadeInDurn);
	}
}

void AMyGameModeBase::WinGame()
{
	OnGameEnd.Broadcast(true);

	bEnded = true;
	
	if (WinSound)
	{
		FVector Location = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WinSound, Location);
	}
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, this, &AMyGameModeBase::GoToMainMenu, EndGameDelay, false);
}

void AMyGameModeBase::LoseGame()
{
	OnGameEnd.Broadcast(false);

	bEnded = true;
	
	if (LoseSound)
	{
		FVector Location = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), LoseSound, Location);
	}
		
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, this, &AMyGameModeBase::RestartLevel, EndGameDelay, false);
}

void AMyGameModeBase::Tick(float DeltaTime)
{
	if (bEnded)
		return;
	
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

void AMyGameModeBase::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AMyGameModeBase::GoToMainMenu()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuLevel"), false);
}