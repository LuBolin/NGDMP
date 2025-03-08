// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryHUD.h"
#include "MasterPlayerController.h"
#include "PickupActor.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
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

	if (PossessedMarble)
	{
		PossessedMarbleInfo->SetVisibility(ESlateVisibility::Visible);
		
		AnimalName->SetText(FText::FromString(PossessedMarble->AnimalDataAsset->AnimalName));
		AbilityName->SetText(FText::FromString(PossessedMarble->AnimalDataAsset->AbilityName));
		AbilityDescription->SetText(FText::FromString(PossessedMarble->AnimalDataAsset->AbilityDescription));
		
		CanUseAbility->SetText(FText::FromString(PossessedMarble->bCanUseAbility ? "Yes" : "No"));
	} else // not possessing a marble
	{
		PossessedMarbleInfo->SetVisibility(ESlateVisibility::Hidden);
	}

	SyncMarblesStatus();
}

void UPrimaryHUD::SetMarble(ABaseMarble* InMarble)
{
	PossessedMarble = InMarble;
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

UTextBlock* UPrimaryHUD::CreateMarbleInfoBlock(FString Data, FLinearColor Color)
{
	UTextBlock* TextBlock = NewObject<UTextBlock>(this);
	TextBlock->SetText(FText::FromString(Data));
	TextBlock->SetMargin(FMargin(10.0f, 10.0f));
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	return TextBlock;
}

void UPrimaryHUD::SyncMarblesStatus()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (!TurnBasedGameState)
		return;

	// array of 4 strings: animal, speed, health. status
	// add them to the grid panel
	FriendlyMarbleInfo->ClearChildren();
	UTextBlock *AnimalText, *SpeedText, *HealthText, *StatusText;
	AnimalText = CreateMarbleInfoBlock("Animal", FLinearColor::White);
	SpeedText = CreateMarbleInfoBlock("Speed", FLinearColor::White);
	HealthText = CreateMarbleInfoBlock("Health", FLinearColor::White);
	StatusText = CreateMarbleInfoBlock("Status", FLinearColor::White);
	FriendlyMarbleInfo->AddChildToGrid(AnimalText, 0, 0);
	FriendlyMarbleInfo->AddChildToGrid(SpeedText, 0, 1);
	FriendlyMarbleInfo->AddChildToGrid(HealthText, 0, 2);
	FriendlyMarbleInfo->AddChildToGrid(StatusText, 0, 3);

	int row = 1;
	for (auto MarblePair : TurnBasedGameState->PlayerMarblesActable)
	{
		ABaseMarble* Marble = MarblePair.Key;
		FString AName = Marble->AnimalDataAsset->AnimalName;
		FString Speed = FString::Printf(TEXT("%.1f"), Marble->GetVelocity().Size());
		FString Health = FString::Printf(TEXT("%.1f"), static_cast<float>(Marble->CombatComponent->CurrentHealth));
		FString Status = Marble->bDead ? "Dead" : Marble->bReadyToLaunch ? "Ready" : "Not Ready";

		FLinearColor FriendlyColor = FLinearColor::Green;
		FLinearColor InfoTextColor = Marble->bDead ? FLinearColor::Gray : FLinearColor::White;
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(AName, FriendlyColor), row, 0);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Speed, InfoTextColor), row, 1);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Health, InfoTextColor), row, 2);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Status, InfoTextColor), row, 3);
		row += 1;
	}

	row += 1;
	
	for (auto MarblePair : TurnBasedGameState->EnemyActorsActable)
	{
		ABaseMarble* Marble = MarblePair.Key;
		FString AName = Marble->AnimalDataAsset->AnimalName;
		FString Speed = FString::Printf(TEXT("%.1f"), Marble->GetVelocity().Size());
		FString Health = FString::Printf(TEXT("%.1f"), static_cast<float>(Marble->CombatComponent->CurrentHealth));
		FString Status = Marble->bDead ? "Dead" : Marble->bReadyToLaunch ? "Ready" : "Not Ready";
		
		FLinearColor EnemyColor = FLinearColor::Red;
		FLinearColor InfoTextColor = Marble->bDead ? FLinearColor::Gray : FLinearColor::White;
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(AName, EnemyColor), row, 0);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Speed, InfoTextColor), row, 1);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Health, InfoTextColor), row, 2);
		FriendlyMarbleInfo->AddChildToGrid(CreateMarbleInfoBlock(Status, InfoTextColor), row, 3);
		row += 1;
	}
}