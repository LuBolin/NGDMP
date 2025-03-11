// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// This component does not tick, it can work on purely event driven logic
	PrimaryComponentTick.bCanEverTick = false;
	
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// Not really needed, since SetMaxHealth will be called based on AnimalData anyways
    CurrentHealth = MaxHealth;
}


void UCombatComponent::TakeDamage(float Damage, AActor* Source)
{
	if (Damage <= 0.f || CurrentHealth <= 0.f)
		return;
	
	// if (Source)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("%s is taking %f damage from %s"), *GetOwner()->GetName(), Damage, *Source->GetName());
	// } else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Taking %f damage"), Damage);	
	// }
	

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth);

	if (CurrentHealth <= 0.f)
	{
		OnDeath.Broadcast();
	}
}

void UCombatComponent::Heal(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Healing %f"), Amount);
	
	if (Amount <= 0.f || CurrentHealth <= 0.f)
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UCombatComponent::SetMaxHealth(float NewMaxHealth, bool Sync)
{
	MaxHealth = NewMaxHealth;
	float NewCurrentHealth;
	if (Sync)
		NewCurrentHealth = MaxHealth;
	else
		NewCurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	
	if (NewCurrentHealth != CurrentHealth)
	{
		CurrentHealth = NewCurrentHealth;
		OnHealthChanged.Broadcast(CurrentHealth);
	}
}