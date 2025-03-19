// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageDealer.generated.h"

UCLASS()
class NGDMP_API ADamageDealer : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageDealer();
	
	UFUNCTION(BlueprintCallable)
	void OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	float CalculateDamage(FVector Velocity, FVector TargetDirn);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 30.0f;

	UPROPERTY()
	FVector LastVelocity;
};
