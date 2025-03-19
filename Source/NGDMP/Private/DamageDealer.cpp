// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageDealer.h"

#include "BaseMarble.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ADamageDealer::ADamageDealer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADamageDealer::BeginPlay()
{
	Super::BeginPlay();
}


void ADamageDealer::OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Handle only marble on marble for now
	if (not OtherActor->IsA<ABaseMarble>())
		return;
	
	ABaseMarble *OtherMarble = Cast<ABaseMarble>(OtherActor);

	// if (CollisionParticle)
	// {
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionParticle, Hit.ImpactPoint);
	// }

	FVector OtherLastVelocity = OtherMarble->LastVelocity;
	FVector TargetDirn = -Hit.ImpactNormal;
	float DamageByOtherVel = CalculateDamage(OtherLastVelocity, TargetDirn);
	
	FVector OwnHitDirn = Hit.ImpactNormal;
	float DamageByOwnVel = CalculateDamage(LastVelocity, OwnHitDirn);
	
	float Damage = DamageByOtherVel + DamageByOwnVel;

	// if damage exists ( >0), then add base damage
	if (Damage > 0.0f)
		Damage += BaseDamage;
	
	OtherMarble->CombatComponent->TakeDamage(Damage, this);

	// if Other Marble is still moving towards self,
	// bounce it away about the hit normal
	// to prevent multiple hits
	FVector SelfToOther = OtherMarble->GetActorLocation() - GetActorLocation();
	float Projection = FVector::DotProduct(LastVelocity, SelfToOther);
	if (Projection > 0)
	{
		FVector BounceDirn = FVector::VectorPlaneProject(LastVelocity, Hit.ImpactNormal);
		OtherMarble->Launch(BounceDirn, LastVelocity.Size(), 0.0f);
	}
}


float ADamageDealer::CalculateDamage(FVector Velocity, FVector TargetDirn)
{
	// if Velocity is approximately zero, return zero damage
	if (Velocity.IsNearlyZero())
		return 0.0f;
	
	// get velocity along the direction of the target
	float ContributingSpeed = FVector::DotProduct(Velocity, TargetDirn);
	float Damage = 0.5f * FMath::Pow((ContributingSpeed / 100.0), 2.0);
	Damage *= DamageMultiplier;
	return Damage;
}

void ADamageDealer::Tick(float DeltaTime)
{
	LastVelocity = GetVelocity();
}

