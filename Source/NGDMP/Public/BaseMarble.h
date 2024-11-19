// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseMarble.generated.h"

UCLASS()
class NGDMP_API ABaseMarble : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseMarble();

	// function to return the plane it is on
	UFUNCTION(BlueprintCallable)
	FVector GetPlaneNormal();

	// default mesh size
	float Radius = 0.0f;

	UPROPERTY()
	UStaticMeshComponent* MarbleMesh = nullptr;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marble")
	bool bPossessed = false;

	UFUNCTION(BlueprintCallable, Category = "Marble")
	void Launch(FVector Direction, float Force);
};
