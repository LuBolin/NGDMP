// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMarble.h"
#include "Components/SphereComponent.h"

ABaseMarble::ABaseMarble()
{
	PrimaryActorTick.bCanEverTick = true;
	PhysicsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Marble"));
	RootComponent = PhysicsMesh;
	AnimalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnimalMesh"));
	AnimalMesh->SetupAttachment(PhysicsMesh);
	MarbleCollider = CreateDefaultSubobject<USphereComponent>(TEXT("MarbleCollider"));
	MarbleCollider->SetupAttachment(PhysicsMesh);
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(PhysicsMesh);
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(CameraSpringArm);
}

// Called when the game starts or when spawned
void ABaseMarble::BeginPlay()
{
	Super::BeginPlay();
	Radius = MarbleCollider->GetUnscaledSphereRadius();
}

// Called every frame
void ABaseMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!ReadyToLaunch and GetVelocity().Size() == 0.0f)
		ReadyToLaunch = true;
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

void ABaseMarble::Launch(FVector Direction, float Velocity, float BlendDelay)
{
	// TODO: Consider using force instead of velocity
	Direction.Normalize();
	FVector LaunchVelocity = Direction * Velocity;
	LaunchVelocity *= 1.0f;
	FRotator NewRotation = LaunchVelocity.Rotation();
	SetActorRotation(NewRotation);
	// set camera rotation separately, since camera
	// does not inherit rotation (as intended) due to spring arm
	CameraSpringArm->SetRelativeRotation(NewRotation);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, LaunchVelocity]()
	{
		PhysicsMesh->SetPhysicsLinearVelocity(LaunchVelocity);
		ReadyToLaunch = false;
	}, BlendDelay, false);
	
}

