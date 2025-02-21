// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryHUD.h"
#include "MasterPlayerController.h"
#include "PickupActor.h"
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
	
	for (auto& Enemy : TurnBasedGameState->EnemyActorsActable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Binding to %s"), *Enemy.Key->GetName());	
		Enemy.Key->CombatComponent->OnDeath.AddDynamic(this, &UPrimaryHUD::SyncEnemyCount);
	}

	SyncEnemyCount();

	
	for (APickupActor* Pickup : TurnBasedGameState->PickupObjectives)
	{
		Pickup->OnPickup.AddDynamic(this, &UPrimaryHUD::SyncPickupObjectiveCount);
	}
	
	SyncPickupObjectiveCount();
	
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

void UPrimaryHUD::SyncPickupObjectiveCount()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;
	
	int32 Count = 0;
	for (APickupActor* Pickup : TurnBasedGameState->PickupObjectives)
	{
		if (not Pickup->bCollected)
			Count++;
	}

	PickupObjectivesCount->SetText(FText::FromString(FString::FromInt(Count) + " pickups left"));
}