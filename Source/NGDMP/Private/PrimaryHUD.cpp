// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryHUD.h"
#include "MasterPlayerController.h"
#include "MaterialHLSLTree.h"
#include "MyGameModeBase.h"
#include "PickupActor.h"
#include "Components/HorizontalBoxSlot.h"
#include "TurnBasedGameState.h"

void UPrimaryHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	// listen to player's FPossess_Updated event
	AMasterPlayerController* MasterPlayerController = AMasterPlayerController::Instance;
	MasterPlayerController->FPossess_Updated.AddDynamic(this, &UPrimaryHUD::SetMarble);
	MasterPlayerController->FState_Updated.AddDynamic(this, &UPrimaryHUD::UpdateStateLabel);
	
	SetMarble(nullptr);

	FinishOverlay->SetVisibility(ESlateVisibility::Hidden);
	InfoBanner->SetVisibility(ESlateVisibility::Hidden);

	AMyGameModeBase* GameMode = AMyGameModeBase::GetInstance();
	GameMode->OnGameEnd.AddDynamic(this, &UPrimaryHUD::ShowFinishOverlay);
	GameMode->PrimaryHUD = this;
	
	ATurnBasedGameState* GameState = ATurnBasedGameState::GetInstance();
	GameState->F_TurnStarted.AddDynamic(this, &UPrimaryHUD::ShowTurnTransition);
	
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

	SyncObjectives();
}

void UPrimaryHUD::SetMarble(ABaseMarble* InMarble)
{
	PossessedMarble = InMarble;
}

void UPrimaryHUD::UpdateStateLabel(FString InStateName)
{
	PlayerState->SetText(FText::FromString(InStateName));
}

UTextBlock* UPrimaryHUD::CreateTextBlock(FString Data, FLinearColor Color)
{
	UTextBlock* TextBlock = NewObject<UTextBlock>(this);
	TextBlock->SetText(FText::FromString(Data));
	TextBlock->SetMargin(FMargin(10.0f, 10.0f));
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	return TextBlock;
}

void UPrimaryHUD::SyncMarblesStatus()
{
	ATurnBasedGameState* TurnBasedGameState = ATurnBasedGameState::GetInstance();
	if (!TurnBasedGameState) return;

	// array of 4 strings: animal, speed, health. status
	// add them to the grid panel
	FriendlyMarbleInfo->ClearChildren();
	UTextBlock *AnimalText, *SpeedText, *HealthText, *StatusText;
	AnimalText = CreateTextBlock("Animal", FLinearColor::White);
	SpeedText = CreateTextBlock("Speed", FLinearColor::White);
	HealthText = CreateTextBlock("Health", FLinearColor::White);
	StatusText = CreateTextBlock("Status", FLinearColor::White);
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
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(AName, FriendlyColor), row, 0);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Speed, InfoTextColor), row, 1);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Health, InfoTextColor), row, 2);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Status, InfoTextColor), row, 3);
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
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(AName, EnemyColor), row, 0);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Speed, InfoTextColor), row, 1);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Health, InfoTextColor), row, 2);
		FriendlyMarbleInfo->AddChildToGrid(CreateTextBlock(Status, InfoTextColor), row, 3);
		row += 1;
	}
}

void UPrimaryHUD::SyncObjectives()
{
	AMyGameModeBase* GameMode = AMyGameModeBase::GetInstance();
	ObjectivesGrid->ClearChildren();
	int row = 0;
	if (GameMode->destroyAllEnemies)
	{
		ObjectivesGrid->AddChildToGrid(CreateTextBlock("Destroy Enemies:", FLinearColor::Yellow), row, 0);
		ObjectivesGrid->AddChildToGrid(CreateTextBlock(GameMode->checkDestroyAllEnemiesProgress(), FLinearColor::Yellow), row, 1);
		row += 1;
	}
	if (GameMode->collectAllStars)
	{
		ObjectivesGrid->AddChildToGrid(CreateTextBlock("Collect Stars:", FLinearColor::Yellow), row, 0);
		ObjectivesGrid->AddChildToGrid(CreateTextBlock(GameMode->checkCollectAllStarsProgress(), FLinearColor::Yellow), row, 1);
		row += 1;
	}
}

void UPrimaryHUD::ShowFinishOverlay(bool bWin)
{
	if (bGameEnded)
		return;
	bGameEnded = true;
	
	FinishOverlay->SetVisibility(ESlateVisibility::Visible);
	FString WinText = "You Win! \n Going to main menu";
	FString LoseText = "You Lose! \n Restarting level";
	FinishText->SetText(FText::FromString(bWin ? WinText : LoseText));
	
	// set brush color, while keeping opacity
	float opacity = FinishOverlay->GetBrushColor().A;
	FLinearColor color = bWin ? FLinearColor::Green : FLinearColor::Red;
	color.A = opacity;
	FinishOverlay->SetBrushColor(color);
}

void UPrimaryHUD::ShowTurnTransition(ETurnState TurnState)
{
	FString info = (TurnState == ETurnState::PLAYER_TURN) ? "Player's Turn" : "Enemy's Turn";
	FLinearColor color = (TurnState == ETurnState::PLAYER_TURN) ? FLinearColor::Green : FLinearColor::Red;
	ShowInfoBanner(info, color, turnTransitionDuration);
}

void UPrimaryHUD::ShowInfoBanner(FString info, FLinearColor backgroundColor, float duration)
{
	InfoBanner->SetVisibility(ESlateVisibility::Visible);
	InfoBannerText->SetText(FText::FromString(info));
	
	constexpr float backgroundOpacity = 0.4f;
	backgroundColor.A = backgroundOpacity;
	InfoBanner->SetBrushColor(backgroundColor);

	// Override previous timer
	GetWorld()->GetTimerManager().ClearTimer(InfoBannerTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		InfoBannerTimerHandle, this, &UPrimaryHUD::HideInfoBanner, duration, false);
}

void UPrimaryHUD::HideInfoBanner()
{
	InfoBanner->SetVisibility(ESlateVisibility::Hidden);
}