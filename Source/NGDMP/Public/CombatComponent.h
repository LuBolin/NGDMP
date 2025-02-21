// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float, NewHealth);

// Notes:
// Inflict damage on other party
// So when A collides with B, damaging B will be done in A, and damaging A will be done in B
// This allows terrain to damage marble

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NGDMP_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float Damage, AActor* Source = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float MaxHealth, bool Sync = false);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathDelegate OnDeath;

};
