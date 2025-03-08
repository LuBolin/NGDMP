// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:
	UFUNCTION()
	void WinGame();
	
	UFUNCTION()
	void LoseGame();
	
	UFUNCTION()
	void RestartGame();

	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool destroyAllEnemies = false;
	
	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool collectAllStars = false;
	
	UPROPERTY(EditAnywhere, Category = "WinCondition")
	bool getAllToEscape = false;

protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void checkDestroyAllEnemies();
	
	UFUNCTION()
	void checkCollectAllStarts();
	
	UFUNCTION()
	void checkGetAllToEscape();
};
