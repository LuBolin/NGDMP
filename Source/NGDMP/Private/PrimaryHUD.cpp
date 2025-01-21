// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryHUD.h"
#include "MasterPlayerController.h"

void UPrimaryHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	// listen to player's FPossess_Updated event
	AMasterPlayerController* MasterPlayerController = AMasterPlayerController::Instance;
	MasterPlayerController->FPossess_Updated.AddDynamic(this, &UPrimaryHUD::SetMarble);
	MasterPlayerController->FState_Updated.AddDynamic(this, &UPrimaryHUD::UpdateStateLabel);
	
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
		// AnimalName->SetText(Marble->AnimalDataAsset->AnimalName);
		// AbilityName->SetText(Marble->AnimalDataAsset->AbilityName);
		// AbilityDescription->SetText(Marble->AnimalDataAsset->AbilityDescription);
		
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