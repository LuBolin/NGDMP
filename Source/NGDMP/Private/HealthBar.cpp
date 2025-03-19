// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

#include "BaseEnemy.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CombatComponent->GetOwner()->IsA<ABaseEnemy>())
	{
		HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
	}
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
	// up to 2 decimal places
	FString CurrentHealthString = FString::Printf(TEXT("%.2f"), CombatComponent->CurrentHealth);
	FString MaxHealthString = FString::Printf(TEXT("%.2f"), CombatComponent->MaxHealth);
	CurrentHealthLabel->SetText(FText::FromString(CurrentHealthString));
	MaxHealthLabel->SetText(FText::FromString(MaxHealthString));

}

void UHealthBar::SetCombatComponent(UCombatComponent* InCombatComponent)
{
	CombatComponent = InCombatComponent;
}