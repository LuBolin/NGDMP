// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
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

	if (ImmunityMap.Contains(Source))
		return;

	ImmunityMap.Add(Source, ImmunityDuration);
	
	Damage = FMath::RoundToFloat(Damage * 100.f) / 100.f;
	
		
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth);

	if (CurrentHealth <= 0.0f)
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

// void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	for (auto& Elem : ImmunityMap)
// 	{
// 		Elem.Value -= DeltaTime;
// 		if (Elem.Value <= 0.f)
// 		{
// 			ImmunityMap.Remove(Elem.Key);
// 		}
// 	}
// }


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Collect keys to remove
	TArray<AActor*> KeysToRemove;

	for (auto& Elem : ImmunityMap)
	{
		Elem.Value -= DeltaTime;
		if (Elem.Value <= 0.f)
		{
			KeysToRemove.Add(Elem.Key);
		}
	}

	// Remove outside of loop
	for (const AActor* Key : KeysToRemove)
	{
		ImmunityMap.Remove(Key);
	}
}
