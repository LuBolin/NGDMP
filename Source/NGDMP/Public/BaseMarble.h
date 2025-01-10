// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "CombatComponent.h"
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

	UPROPERTY(BlueprintReadWrite)
	bool ReadyToLaunch = false;

	UPROPERTY(BlueprintReadWrite)
	bool CanUseAbility = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PhysicsMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AnimalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* MarbleCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent *AnimalCameraSpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FirstPersonCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* OutlineMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* InfoSpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* StatusLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCombatComponent* CombatComponent;
	
	UPROPERTY(BlueprintReadWrite)
	bool bPossessed = false;
	
protected:
	const float MaxDistToRenderStatusLabel = 2000.0f;
	
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;
	

private:
	UPROPERTY()
	UMaterialInstanceDynamic* OutlineMaterialInstance;
	
	UFUNCTION()
	void InitOutlineMaterialInstance();

	UFUNCTION()
	void AimUpdateOutlineMaterialInstance(ABaseMarble* AimedMarble);

	UFUNCTION()
	void PossessUpdateOutlineMaterialInstance(ABaseMarble* PossessedMarble);

	UFUNCTION()
	void UpdateStatusValues();

	UFUNCTION()
	void SyncWithVelocityDirection();
	
	UFUNCTION()
	void RenderInfoGroup();

};
