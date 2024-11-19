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



// input events for states to 'subscribe' to in C++
// basically broadcast the IA delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Move, FVector3f, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_MouseLook, FVector2f, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Inspect, bool, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_MainAction, bool, Input);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIA_Escape, bool, Input);

UCLASS()
class NGDMP_API AMasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	static AMasterPlayerController* Instance;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	FAim_Updated FAim_Updated;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Event")
	FPossess_Updated FPossess_Updated;


	UPROPERTY(BlueprintCallable, BlueprintReadOnly, Category = "Input")
	FIA_Move FIA_Move;
	UPROPERTY(BlueprintCallable, BlueprintReadOnly, Category = "Input")
	FIA_MouseLook FIA_MouseLook;
	UPROPERTY(BlueprintCallable, BlueprintReadOnly, Category = "Input")
	FIA_Inspect FIA_Inspect;
	UPROPERTY(BlueprintCallable, BlueprintReadOnly, Category = "Input")
	FIA_MainAction FIA_MainAction;
	UPROPERTY(BlueprintCallable, BlueprintReadOnly, Category = "Input")
	FIA_Escape FIA_Escape;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "General")
	UStateTreeComponent* StateTreeComponent = nullptr;

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Possession")
	APawn* SpectatePawn = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Possession")
	class ABaseMarble* PossessedMarble = nullptr;
	
protected:
	virtual void BeginPlay() override;

};


