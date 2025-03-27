// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PickupActor.generated.h"

// parameter of picking actor
// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupDelegate, ABaseMarble*, PickingMarble);

UCLASS()
class NGDMP_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PickupIndicatorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* PickupCollider;
	
	UPROPERTY(EditAnywhere)
	bool bEnemyInteractable = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bCollected = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* PickupParticle;
	
	UPROPERTY(BlueprintAssignable)
	FOnPickupDelegate OnPickup;

	UPROPERTY() // rotate 1 round every 2 seconds
	float RotationPeriod = 2.0f;
	
	UPROPERTY(EditAnywhere)
	bool bIsObjective = true;

	UPROPERTY(EditAnywhere)
	bool bShouldRotate = true;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere)
	bool bReuseable = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AddToGameModeAndState();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
