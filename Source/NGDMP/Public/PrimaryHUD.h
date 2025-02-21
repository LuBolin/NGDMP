// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "PrimaryHUD.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UPrimaryHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
	ABaseMarble* Marble;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerState;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* AnimalName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityName;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityDescription;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CanUseAbility;
	
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* PossessedMarbleInfo;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* EnemyCount;

private:
	UFUNCTION()
	void SetMarble(ABaseMarble* InMarble);

	UFUNCTION()
	void UpdateStateLabel(FString InStateName);
	
	UFUNCTION()
	void SyncEnemyCount();
};
