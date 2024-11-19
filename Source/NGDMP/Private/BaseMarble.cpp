// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMarble.h"
#include "Components/SphereComponent.h"

// Sets default values
ABaseMarble::ABaseMarble()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseMarble::BeginPlay()
{
	Super::BeginPlay();
	
	Radius = GetComponentByClass<USphereComponent>()->GetUnscaledSphereRadius();

	MarbleMesh = GetComponentByClass<UStaticMeshComponent>();
}

// Called every frame
void ABaseMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FVector ABaseMarble::GetPlaneNormal()
{
	// raycast downwards to get the plane normal
	// of the plane the marble is on
	
	FVector StartLocation = GetActorLocation();
	FVector Down = FVector(0.0f, 0.0f, -1.0f);
	FVector EndLocation = StartLocation + 1000.0f * Down;
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, StartLocation, EndLocation,
		ECollisionChannel::ECC_Visibility, CollisionParams);

	FVector PlaneNormal = FVector::ZeroVector;
	if (bHit)
		PlaneNormal = HitResult.ImpactNormal;
	
	return PlaneNormal;
}

void ABaseMarble::Launch(FVector Direction, float Velocity)
{
	// TODO: Consider using force instead of velocity
	Direction.Normalize();
	FVector LaunchVelocity = Direction * Velocity;
	LaunchVelocity *= 1.0f;
	MarbleMesh->SetPhysicsLinearVelocity(LaunchVelocity);
}

