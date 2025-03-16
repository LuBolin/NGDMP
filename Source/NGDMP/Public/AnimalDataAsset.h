// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimalDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UAnimalDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString AnimalName;

	UPROPERTY(EditAnywhere)
	FString AbilityName;
	
	UPROPERTY(EditAnywhere)
	FString AbilityDescription;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditAnywhere)
	float MaxLaunchForce = 600.0f;
	
	// Not putting mesh here due to scaling
	// Easier to scale in the editor

};
