// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"

#include "BaseEnemy.h"
#include "BaseMarble.h"
#include "MyGameModeBase.h"
#include "NiagaraFunctionLibrary.h"
#include "TurnBasedGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickupActor::APickupActor()
{
	// object mesh is inside so re-scaling would not affect collider
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	PickupCollider->SetCollisionProfileName(TEXT("OverlapAll"));
	RootComponent = PickupCollider;
	PickupIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupIndicatorMesh"));
	PickupIndicatorMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	PickupIndicatorMesh->SetupAttachment(PickupCollider);
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	ObjectMesh->SetupAttachment(PickupCollider);
	
	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnPickupOverlap);
	
	PrimaryActorTick.bCanEverTick = true;
}


void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (bIsObjective)
		bReuseable = false;
	
	AddToGameModeAndState();
}

void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldRotate)
	{
		// rotate ObjectMesh about the Z axis based on RotationPeriod
		float RotationAngle = 360.0f * DeltaTime / RotationPeriod;
		ObjectMesh->AddLocalRotation(FRotator(0.0f, RotationAngle, 0.0f));
	}
}


void APickupActor::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected)
		return;
	
	if (OtherActor->IsA<ABaseMarble>() and not OtherActor->IsA<ABaseEnemy>())
	{
		ABaseMarble* Marble = Cast<ABaseMarble>(OtherActor);
		if (not bReuseable)
		{
			bCollected = true;
			ObjectMesh->SetVisibility(false);
			PickupCollider->SetVisibility(false);
			PickupIndicatorMesh->SetVisibility(false);			
		}
		
		if (PickupParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupParticle, GetActorLocation());
		}
		
		OnPickup.Broadcast(Marble);

		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
		}	
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
