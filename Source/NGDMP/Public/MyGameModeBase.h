// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseMarble.h"
#include "BaseEnemy.h"
#include "PickupActor.h"
#include "MyGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEnd, bool, bHasWon);

/**
 * 
 */
UCLASS()
class NGDMP_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:
	AMyGameModeBase();
	
	UFUNCTION()
	void WinGame();
	
	UFUNCTION()
	void LoseGame();

	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool destroyAllEnemies = true;
	
	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool collectAllStars = true;
	
	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool getAllToEscape = false;

	UPROPERTY()
	TArray<ABaseMarble*> PlayerMarbles;
	
	UPROPERTY()
	TArray<ABaseEnemy*> EnemyActors;

	UPROPERTY()
	TArray<APickupActor*> PickupObjectives;

	static FString ObjectiveCompleteMessage;
	
	UFUNCTION()
	static AMyGameModeBase* GetInstance()
	{
		return Instance;
	}
	
	static AMyGameModeBase* Instance;
		
	UFUNCTION()
	FString checkDestroyAllEnemiesProgress();
	
	UFUNCTION()
	FString checkCollectAllStarsProgress();
	
	// UFUNCTION()
	// void checkGetAllToEscapeProgress();

	UPROPERTY()
	FOnGameEnd OnGameEnd;

	UFUNCTION()
	void RestartLevel();

	UFUNCTION()
	void GoToMainMenu();
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	float EndGameDelay = 3.0f;
	
	UPROPERTY(EditAnywhere)
	USoundBase* StartSound;
	
	UPROPERTY(EditAnywhere)
	USoundBase* WinSound;

	UPROPERTY(EditAnywhere)
	USoundBase* LoseSound;

	UPROPERTY()
	bool bEnded = false;
};
