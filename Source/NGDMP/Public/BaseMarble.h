// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseMarble.generated.h"


UCLASS()
class NGDMP_API ABaseMarble : public APawn
{
	GENERATED_BODY()

public:
	ABaseMarble();
	
	// Returns the plane the marble is on
	UFUNCTION(BlueprintCallable)
	FVector GetPlaneNormal();

	UFUNCTION()
	void Launch(FVector Direction, float Force, float BlendDelay);
	
	UPROPERTY()
	float Radius = 0.0f;

	UPROPERTY()
	bool ReadyToLaunch = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PhysicsMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AnimalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* MarbleCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent *CameraSpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(BlueprintReadWrite)
	bool bPossessed = false;
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

};
