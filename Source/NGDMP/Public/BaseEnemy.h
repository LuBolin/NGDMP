// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "BaseEnemy.generated.h"

UCLASS()
class NGDMP_API ABaseEnemy : public ABaseMarble
{
	GENERATED_BODY()
	
public:
	
	ABaseEnemy();

	// an act function that is to be implemented in blueprint
	// UFUNCTION(BlueprintImplementableEvent)
	UFUNCTION()
	void Act();

protected:

	virtual void BeginPlay() override;

	virtual void EndTurn() override;
	
public:
	
	virtual void Tick(float DeltaTime) override;

};
