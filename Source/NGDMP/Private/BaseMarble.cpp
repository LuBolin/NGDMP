// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMarble.h"
#include "HealthBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "MasterPlayerController.h"
#include "TurnBasedGameState.h"


ABaseMarble::ABaseMarble()
{
	PrimaryActorTick.bCanEverTick = true;
	PhysicsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Marble"));
	PhysicsMesh->SetSimulatePhysics(true);
	RootComponent = PhysicsMesh;
	MarbleCollider = CreateDefaultSubobject<USphereComponent>(TEXT("MarbleCollider"));
	MarbleCollider->SetupAttachment(PhysicsMesh);
	AnimalCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	AnimalCameraSpringArm->TargetArmLength = 0;
	AnimalCameraSpringArm->SetupAttachment(PhysicsMesh);
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(AnimalCameraSpringArm);
	AnimalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnimalMesh"));
	AnimalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// AnimalMesh->SetupAttachment(PhysicsMesh);
	AnimalMesh->SetupAttachment(AnimalCameraSpringArm);
	OutlineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutlineMesh"));
	OutlineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OutlineMesh->SetupAttachment(PhysicsMesh);
	InfoSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("InfoSpringArm"));
	InfoSpringArm->TargetArmLength = 0;
	InfoSpringArm->TargetOffset = FVector::UpVector * 50.0f;
	InfoSpringArm->SetupAttachment(PhysicsMesh);
	StatusLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StatusLabel"));
	StatusLabel->SetupAttachment(InfoSpringArm);
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
	HealthBar->SetCastShadow(false);
	HealthBar->SetDrawAtDesiredSize(true);
	HealthBar->SetPivot(FVector2D(0.5f, 0.5f));
	HealthBar->SetupAttachment(InfoSpringArm);
	
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}

// Called when the game starts or when spawned
void ABaseMarble::BeginPlay()
{
	Super::BeginPlay();
	
	AMasterPlayerController* MasterPlayerController = AMasterPlayerController::Instance;

	InitComponents();
	
	MasterPlayerController->FAim_Updated.AddDynamic(this, &ABaseMarble::AimUpdateOutlineMaterialInstance);
	MasterPlayerController->FPossess_Updated.AddDynamic(this, &ABaseMarble::PossessUpdateOutlineMaterialInstance);
	
	ReadyToLaunch = true;

	AddToGameState();
}

// Called every frame
void ABaseMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStatusValues();
	
	SyncWithVelocityDirection();

	RenderInfoGroup();
}


FVector ABaseMarble::GetPlaneNormal()
{
	// raycast downwards to get the plane normal
	// of the plane the marble is on
	
	FVector StartLocation = GetActorLocation();
	FVector Down = FVector(0.0f, 0.0f, -1.0f);
	FVector EndLocation = StartLocation + 1000.0f * Down;
	
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, StartLocation, EndLocation,
		ECollisionChannel::ECC_Visibility, CollisionParams);

	FVector PlaneNormal = FVector::ZeroVector;
	if (bHit)
		PlaneNormal = HitResult.ImpactNormal;
	
	return PlaneNormal;
}

void ABaseMarble::Launch(FVector Direction, float Velocity, float BlendDelay)
{
	// TODO: Consider using force instead of velocity
	Direction.Normalize();
	FVector LaunchVelocity = Direction * Velocity;
	LaunchVelocity *= 1.0f;
	FRotator NewRotation = LaunchVelocity.Rotation();
	SetActorRotation(NewRotation);
	// set camera rotation separately, since camera
	// does not inherit rotation (as intended) due to spring arm
	AnimalCameraSpringArm->SetRelativeRotation(NewRotation);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, LaunchVelocity]()
	{
		PhysicsMesh->SetPhysicsLinearVelocity(LaunchVelocity);
		TakingTurn = true;
		ReadyToLaunch = false;
		CanUseAbility = true;
	}, BlendDelay, false);
}

void ABaseMarble::InitOutlineMaterialInstance()
{
	UMaterialInterface* OutlineMaterial = OutlineMesh->GetMaterial(0);
	
	if (!OutlineMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("OutlineMaterial is not valid"));
		return;
	}
	
	OutlineMaterialInstance = OutlineMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, OutlineMaterial);
	
	FLinearColor Black = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	OutlineMaterialInstance->SetVectorParameterValue("OutlineColor", Black);
	
	OutlineMesh->SetOverlayMaterial(OutlineMaterialInstance);
}

void ABaseMarble::AimUpdateOutlineMaterialInstance(ABaseMarble* AimedMarble)
{
	if (bPossessed)
		return;

	FLinearColor OutlineColor = FLinearColor::Black;
	if (AimedMarble == this)
		OutlineColor = FLinearColor::Yellow;

	OutlineMaterialInstance->SetVectorParameterValue("OutlineColor", OutlineColor);
}

void ABaseMarble::PossessUpdateOutlineMaterialInstance(ABaseMarble* PossessedMarble)
{
	bPossessed = PossessedMarble == this;
	
	FLinearColor OutlineColor = FLinearColor::Black;
	if (bPossessed)
		OutlineColor = FLinearColor::Green;

	OutlineMaterialInstance->SetVectorParameterValue("OutlineColor", OutlineColor);
}

void ABaseMarble::UpdateStatusValues()
{
	bool Awake = PhysicsMesh->IsAnyRigidBodyAwake();
	if (TakingTurn and not Awake) // physics has been put to sleep due to Physics Material's Sleep Threshold
	{
		// Marble has stopped
		EndTurn();
	}
}

void ABaseMarble::SyncWithVelocityDirection()
{
	FVector Velocity = PhysicsMesh->GetPhysicsLinearVelocity();
	if (Velocity.Size() > 10.0f) // arbitrary threshold to avoid jittering, to be tuned later
	{
		SetActorRotation(Velocity.Rotation());
	}
}

void ABaseMarble::RenderInfoGroup()
{
	AMasterPlayerController* PlayerController = AMasterPlayerController::Instance;
	FVector PlayerCameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	FVector OwnLocation = GetActorLocation();
	float Distance = FVector::Dist(PlayerCameraLocation, OwnLocation);
	bool ShouldRender = Radius < Distance && Distance <= MaxDistToRenderStatusLabel;

	if (ShouldRender)
	{
		FString StatusString = "";
		StatusString += ReadyToLaunch ? "Ready to launch\n" : "Not ready to launch\n";
		StatusString += CanUseAbility ? "Can use ability\n" : "Cannot use ability\n";
		// StatusString += "Velocity: " + FString::SanitizeFloat(GetVelocity().Size());
		float LinearVelocity = PhysicsMesh->GetPhysicsLinearVelocity().Size();
		float AngularVelocity = PhysicsMesh->GetPhysicsAngularVelocityInRadians().Size();
		StatusString += "Velocity: " + FString::SanitizeFloat(LinearVelocity) + "\n";
		StatusString += "Angular Velocity: " + FString::SanitizeFloat(AngularVelocity);
		
		StatusLabel->SetText(FText::FromString(StatusString));
		
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnLocation, PlayerCameraLocation);
		StatusLabel->SetWorldRotation(LookAtRotation);
		HealthBar->SetWorldRotation(LookAtRotation);
	}
	StatusLabel->SetVisibility(ShouldRender);
}

void ABaseMarble::EndTurn()
{
	ReadyToLaunch = true;
	CanUseAbility = false;
	TakingTurn = false;
	F_OnStopActing.Broadcast(this);
}

void ABaseMarble::InitComponents()
{
	Radius = MarbleCollider->GetUnscaledSphereRadius();

	if (!AnimalDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimalDataAsset is not valid"));
		Destroy();
		return;
	}
	CombatComponent->SetMaxHealth(AnimalDataAsset->MaxHealth, true);
	
	InitOutlineMaterialInstance();

	InfoSpringArm->TargetOffset += FVector::UpVector * Radius * 1.0f;
	// SetupAttachment does not work in constructor for this for some reason
	StatusLabel->AttachToComponent(InfoSpringArm, FAttachmentTransformRules::KeepRelativeTransform);

	UHealthBar* HealthBarWidget = Cast<UHealthBar>(HealthBar->GetUserWidgetObject());
	HealthBarWidget->SetCombatComponent(CombatComponent);
}

void ABaseMarble::AddToGameState()
{
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (not GameState)
		return;
	
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;

	if (IsA<ABaseEnemy>())
		TurnBasedGameState->EnemyActorsActable.Add(Cast<ABaseEnemy>(this), false);
	else
		TurnBasedGameState->PlayerMarblesActable.Add(this, false);
}
