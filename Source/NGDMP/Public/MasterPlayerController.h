// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MasterPlayerController.generated.h"


// UENUM(BlueprintType)
// enum class EPlayerControllerState: uint8
// {
// 	Pcs_Free, // Not possessing a marble
// 	Pcs_Possessing, // Has possessed a marble
// 	Pcs_Launching, // Third person aiming marble
// 	Pcs_Acting // First Person
// };


UCLASS()
class NGDMP_API AMasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	static AMasterPlayerController* Instance;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marble")
	bool bFocusedOnMarble = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marble")
	class ABaseMarble* PossessedMarble = nullptr;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Marble")
	// EPlayerControllerState CurrentState = EPlayerControllerState::Pcs_Free;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void CameraMovement(FVector3f Input);
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void ToggleFocusOnMarble();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void CameraPan(FVector2f Input);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual void SetMarblePerspective(bool bFirstPerson);


	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float CameraSpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	APawn* CameraPawn = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Camera")
	bool FirstPerson = false;

private:
	float MinCamDist = 300.0f;
	float MaxCamDist = 1000.0f;
	float StoredCamDist = 0.0f;
};


