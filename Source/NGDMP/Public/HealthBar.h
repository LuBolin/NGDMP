// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "CombatComponent.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetCombatComponent(UCombatComponent* CombatComponent);
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	UCombatComponent* CombatComponent;
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CurrentHealthLabel;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MaxHealthLabel;
};
