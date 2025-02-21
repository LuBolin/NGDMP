// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"

#include "BaseEnemy.h"
#include "BaseMarble.h"
#include "NiagaraFunctionLibrary.h"
#include "TurnBasedGameState.h"

// Sets default values
APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ObjectMesh;
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	PickupCollider->SetupAttachment(ObjectMesh);
	
	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnPickupOverlap);
}


void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	AddToGameState();
}

void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APickupActor::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected)
		return;
	if (OtherActor->IsA<ABaseMarble>() and not OtherActor->IsA<ABaseEnemy>())
	{
		ABaseMarble* Marble = Cast<ABaseMarble>(OtherActor);
		Marble->CombatComponent->Heal(100);
		bCollected = true;
		ObjectMesh->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("%s collected %s"), *Marble->GetName(), *GetName());
		if (PickupParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupParticle, GetActorLocation());
		}
		OnPickup.Broadcast();
	}
}


void APickupActor::AddToGameState()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;

	TurnBasedGameState->PickupObjectives.Add(this);
}
