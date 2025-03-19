// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "TurnBasedGameState.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/GridPanel.h"
#include "PrimaryHUD.generated.h"

/**
 * 
 */
UCLASS()
class NGDMP_API UPrimaryHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	static constexpr float turnTransitionDuration = 1.2f;

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
	ABaseMarble* PossessedMarble;

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
	UGridPanel* FriendlyMarbleInfo;
	
	// UPROPERTY(meta=(BindWidget))
	// UTextBlock* EnemyCount;
	//
	// UPROPERTY(meta=(BindWidget))
	// UTextBlock* PickupObjectivesCount;

	UPROPERTY(meta=(BindWidget))
	UGridPanel* ObjectivesGrid;

	UPROPERTY(meta=(BindWidget))
	UBorder* FinishOverlay;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* FinishText;
	
	UPROPERTY(meta=(BindWidget))
	UBorder* InfoBanner;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* InfoBannerText;
	
private:
	UFUNCTION()
	void SetMarble(ABaseMarble* InMarble);

	UFUNCTION()
	void UpdateStateLabel(FString InStateName);
	
	UFUNCTION()
	void SyncMarblesStatus();

	UFUNCTION()
	UTextBlock* CreateTextBlock(FString Data, FLinearColor Color = FLinearColor::White);
	
	UFUNCTION()
	void SyncObjectives();

	UFUNCTION()
	void ShowFinishOverlay(bool bWin);

	UPROPERTY()
	bool bGameEnded = false;
	
	UFUNCTION()
	void ShowTurnTransition(ETurnState TurnState);
	
	UFUNCTION()
	void ShowInfoBanner(FString info, FLinearColor backgroundColor, float duration);
	
	UFUNCTION()
	void HideInfoBanner();
};
