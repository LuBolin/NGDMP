// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryHUD.h"
#include "MasterPlayerController.h"
#include "TurnBasedGameState.h"

void UPrimaryHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	// listen to player's FPossess_Updated event
	AMasterPlayerController* MasterPlayerController = AMasterPlayerController::Instance;
	MasterPlayerController->FPossess_Updated.AddDynamic(this, &UPrimaryHUD::SetMarble);
	MasterPlayerController->FState_Updated.AddDynamic(this, &UPrimaryHUD::UpdateStateLabel);

	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;
	
	// iterate all enemy marble, and bind their onDeath to 
	for (auto& Enemy : TurnBasedGameState->EnemyActorsActable)
	{
		Enemy.Key->CombatComponent->OnDeath.AddDynamic(this, &UPrimaryHUD::SyncEnemyCount);
	}

	SyncEnemyCount();
	
	SetMarble(nullptr);
}

// Change this to binding if performance is an issue
void UPrimaryHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Marble)
	{
		PossessedMarbleInfo->SetVisibility(ESlateVisibility::Visible);
		
		AnimalName->SetText(FText::FromString(Marble->AnimalDataAsset->AnimalName));
		AbilityName->SetText(FText::FromString(Marble->AnimalDataAsset->AbilityName));
		AbilityDescription->SetText(FText::FromString(Marble->AnimalDataAsset->AbilityDescription));
		
		CanUseAbility->SetText(FText::FromString(Marble->CanUseAbility ? "Yes" : "No"));
	} else // not possessing a marble
	{
		PossessedMarbleInfo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPrimaryHUD::SetMarble(ABaseMarble* InMarble)
{
	Marble = InMarble;
}

void UPrimaryHUD::UpdateStateLabel(FString InStateName)
{
	PlayerState->SetText(FText::FromString(InStateName));
}

void UPrimaryHUD::SyncEnemyCount()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;
	
	int32 Count = 0;
	for (auto& Enemy : TurnBasedGameState->EnemyActorsActable)
	{
		if (not Enemy.Key->bDead)
			Count++;
	}

	// X enemies left
	EnemyCount->SetText(FText::FromString(FString::FromInt(Count) + " enemies left"));
}
