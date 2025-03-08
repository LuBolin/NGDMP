// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseMarble.h"
#include "BaseEnemy.h"
#include "PickupActor.h"
#include "MyGameModeBase.generated.h"

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
	
	UFUNCTION()
	void RestartGame();

	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool destroyAllEnemies = true;
	
	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool collectAllStars = false;
	
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
	
	// UFUNCTION()
	// void checkCollectAllStartsProgress();
	//
	// UFUNCTION()
	// void checkGetAllToEscapeProgress();
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

};
