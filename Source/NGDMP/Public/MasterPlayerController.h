// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMarble.h"
#include "GameFramework/PlayerController.h"
#include "Components/StateTreeComponent.h"
#include "MasterPlayerController.generated.h"


// signals for other actors to 'subscribe' to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAim_Updated, ABaseMarble*, AimedMarble);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossess_Updated, ABaseMarble*, PossessedMarble);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FState_Updated, FString, StateName);


// input events for states to 'subscribe' to in C++
// basically broadcast the IA delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Move, FVector3f, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_MouseLook, FVector2f, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Toggle, bool, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_MainAction, bool, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Escape, bool, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Interact, bool, Input);

UCLASS()
class NGDMP_API AMasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMasterPlayerController();

	static AMasterPlayerController* Instance;
	
	UFUNCTION()
	static void ExitGame();

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	FAim_Updated FAim_Updated;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	FPossess_Updated FPossess_Updated;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	FState_Updated FState_Updated;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_Move FIA_Move;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_MouseLook FIA_MouseLook;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_Toggle FIA_Toggle;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_MainAction FIA_MainAction;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_Escape FIA_Escape;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Input")
	FIA_Interact FIA_Interact;

	UFUNCTION(BlueprintCallable) // helper function to send a state tree event by tag
	void SendStateTreeEventByTagString(FString TagString);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General")
	UStateTreeComponent* StateTreeComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Possession")
	APawn* SpectatePawn = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Possession")
	ABaseMarble* PossessedMarble = nullptr;
	
protected:
	UFUNCTION()
	virtual void BeginPlay() override;

};