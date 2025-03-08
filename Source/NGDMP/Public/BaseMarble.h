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
#include "AnimalDataAsset.h"
#include "NiagaraSystem.h"
#include "BaseMarble.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F_OnStopActing, ABaseMarble*, ActingMarble);

UCLASS()
class NGDMP_API ABaseMarble : public APawn
{
	GENERATED_BODY()

public:
	ABaseMarble();

	UPROPERTY()
	float Radius = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bReadyToLaunch = false;

	UPROPERTY(BlueprintReadWrite)
	bool bCanUseAbility = false;
	
	UPROPERTY(BlueprintReadWrite)
	bool bTakingTurn = false;

	UPROPERTY(BlueprintReadOnly)
	bool bPossessed = false;

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false;


		
	// Returns the plane the marble is on
	UFUNCTION(BlueprintCallable)
	FVector GetPlaneNormal();

	UFUNCTION()
	void Launch(FVector Direction, float Force, float BlendDelay);


	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimalDataAsset* AnimalDataAsset;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	F_OnStopActing F_OnStopActing;

	
	UFUNCTION()
	virtual void GetReadyForNewTurn();
	
	UFUNCTION()
	virtual void CleanUpForEndTurn();

	UFUNCTION()
	virtual void EndTurn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* CollisionParticle;
	
protected:
	const float MaxDistToRenderStatusLabel = 2000.0f;
	
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION()
	FVector GetRandomVectorOnPlane(FVector Normal);
	
	UPROPERTY()
	FLinearColor NeutralOutlineColor = FLinearColor::Black;

	UPROPERTY()
	FLinearColor AimedOutlineColor = FLinearColor::Yellow;
	
	UPROPERTY()
	FLinearColor PossessedOutlineColor = FLinearColor::Green;

	UFUNCTION()
	void Die();
	
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

	UFUNCTION()
	void InitComponents();

	UFUNCTION()
	void AddToGameModeAndState();

	UFUNCTION()
	void SolvePhysicsSleep();

	UPROPERTY()
	float SleepLinearThreshold = 20.0f; // very slow

	UPROPERTY()
	int SleepCounterThreshold = 15; // quarter second
	
	UPROPERTY()
	int SleepCounter = 0;

	UPROPERTY()
	FVector LastVelocity = FVector::ZeroVector;

	UFUNCTION()
	void OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	float CalculateDamage(FVector Velocity, FVector TargetDirn);
};
