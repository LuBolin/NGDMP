// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"

#include "BaseEnemy.h"
#include "BaseMarble.h"
#include "MyGameModeBase.h"
#include "NiagaraFunctionLibrary.h"
#include "TurnBasedGameState.h"

// Sets default values
APickupActor::APickupActor()
{
	UE_LOG(LogTemp, Warning, TEXT("A"));
	// object mesh is inside so re-scaling would not affect collider
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	PickupCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = PickupCollider;
	PickupIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupIndicatorMesh"));
	PickupIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupIndicatorMesh->SetupAttachment(PickupCollider);
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ObjectMesh->SetupAttachment(PickupCollider);
	
	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnPickupOverlap);
	
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(LogTemp, Warning, TEXT("B"));
}


void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	AddToGameModeAndState();
}

void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// rotate ObjectMesh about the Z axis based on RotationPeriod
	float RotationAngle = 360.0f * DeltaTime / RotationPeriod;
	ObjectMesh->AddLocalRotation(FRotator(0.0f, RotationAngle, 0.0f));
}


void APickupActor::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected)
		return;
	if (OtherActor->IsA<ABaseMarble>() and not OtherActor->IsA<ABaseEnemy>())
	{
		ABaseMarble* Marble = Cast<ABaseMarble>(OtherActor);
		bCollected = true;
		ObjectMesh->SetVisibility(false);
		PickupCollider->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("%s collected %s"), *Marble->GetName(), *GetName());
		if (PickupParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupParticle, GetActorLocation());
		}
		OnPickup.Broadcast();
	}
}


void APickupActor::AddToGameModeAndState()
{
	if (not bIsObjective) return;
	
	ATurnBasedGameState* TurnBasedGameState = ATurnBasedGameState::GetInstance();
	if (!TurnBasedGameState) return;
	AMyGameModeBase* GameMode = AMyGameModeBase::GetInstance();
	
	TurnBasedGameState->PickupObjectives.Add(this);
	GameMode->PickupObjectives.Add(this);
}
