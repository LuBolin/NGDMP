// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CombatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatComponent is not valid"));
		return;
	}

	HealthBar->SetPercent(CombatComponent->CurrentHealth / CombatComponent->MaxHealth);
	CurrentHealthLabel->SetText(FText::FromString(FString::SanitizeFloat(CombatComponent->CurrentHealth)));
	MaxHealthLabel->SetText(FText::FromString(FString::SanitizeFloat(CombatComponent->MaxHealth)));

}

void UHealthBar::SetCombatComponent(UCombatComponent* InCombatComponent)
{
	CombatComponent = InCombatComponent;
}