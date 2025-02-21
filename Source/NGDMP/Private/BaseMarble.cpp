// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMarble.h"
#include "HealthBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "MasterPlayerController.h"
#include "NiagaraFunctionLibrary.h"
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

	PhysicsMesh->OnComponentHit.AddDynamic(this, &ABaseMarble::OnPhysicsHit);
	CombatComponent->OnDeath.AddDynamic(this, &ABaseMarble::Die);
	
	// Game State will set this to true when it is the marble's turn
	ReadyToLaunch = false;

	bDead = false;

	AddToGameState();
}

// Called every frame
void ABaseMarble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead)
		return;
	
	UpdateStatusValues();
	
	SyncWithVelocityDirection();

	RenderInfoGroup();

	SolvePhysicsSleep();
}


void ABaseMarble::SolvePhysicsSleep()
{
	FVector CurrentVelocity = PhysicsMesh->GetPhysicsLinearVelocity();
	if (PhysicsMesh->IsAnyRigidBodyAwake())
	{
		// Check if within threshold and also slowing down
		if (CurrentVelocity.Size() < SleepLinearThreshold and CurrentVelocity.Size() < LastVelocity.Size())
		{
			SleepCounter++;
			if (SleepCounter >= SleepCounterThreshold)
			{
				PhysicsMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
				PhysicsMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
				PhysicsMesh->PutRigidBodyToSleep();
				SleepCounter = 0;
			}
		}
		else
		{
			SleepCounter = 0;
		}
	}
	LastVelocity = CurrentVelocity;
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

	if (BlendDelay <= 0.0f)
	{
		BlendDelay = 0.1f;
	}
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
	
	OutlineMaterialInstance->SetVectorParameterValue("OutlineColor", NeutralOutlineColor);
	
	OutlineMesh->SetOverlayMaterial(OutlineMaterialInstance);
}

void ABaseMarble::AimUpdateOutlineMaterialInstance(ABaseMarble* AimedMarble)
{
	if (bPossessed)
		return;

	FLinearColor OutlineColor = (AimedMarble == this) ? AimedOutlineColor : NeutralOutlineColor;

	OutlineMaterialInstance->SetVectorParameterValue("OutlineColor", OutlineColor);
}

void ABaseMarble::PossessUpdateOutlineMaterialInstance(ABaseMarble* PossessedMarble)
{
	bPossessed = PossessedMarble == this;
	
	FLinearColor OutlineColor = NeutralOutlineColor;
	if (bPossessed)
		OutlineColor = PossessedOutlineColor;

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

void ABaseMarble::GetReadyForNewTurn()
{
	ReadyToLaunch = true;
}

void ABaseMarble::CleanUpForEndTurn()
{
	ReadyToLaunch = false;
	CanUseAbility = false;
	TakingTurn = false;
}

void ABaseMarble::EndTurn()
{
	CleanUpForEndTurn();
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
	ATurnBasedGameState* TurnBasedGameState = Cast<ATurnBasedGameState>(GameState);
	if (not TurnBasedGameState)
		return;
		
	if (IsA<ABaseEnemy>())
		TurnBasedGameState->EnemyActorsActable.Add(Cast<ABaseEnemy>(this), false);
	else
		TurnBasedGameState->PlayerMarblesActable.Add(this, false);
}


FVector ABaseMarble::GetRandomVectorOnPlane(FVector Normal)
{
	Normal.Normalize();

	// Find a perpendicular vector by crossing Normal with an arbitrary axis (Up or Right)
	FVector ArbitraryNonParallelAxis = FMath::IsNearlyEqual(FMath::Abs(Normal.Z), 1.0f, 0.1f) ? FVector::RightVector : FVector::UpVector;
	FVector Perpendicular = Normal ^ ArbitraryNonParallelAxis; // cross product 
	Perpendicular.Normalize();
	
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);

	FVector RandomVector = Perpendicular.RotateAngleAxis(RandomAngle, Normal);

	return RandomVector;
}

void ABaseMarble::OnPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Handle only marble on marble for now
	if (not OtherActor->IsA<ABaseMarble>())
		return;
	
	ABaseMarble *OtherMarble = Cast<ABaseMarble>(OtherActor);

	// Combat only happens between enemies
	bool SameTeam = OtherMarble->IsA<ABaseEnemy>() == IsA<ABaseEnemy>();
	if (SameTeam)
		return;
	
	
	// // print self and self velocity
	UE_LOG(LogTemp, Warning, TEXT("Self: %s"), *GetName());
	// UE_LOG(LogTemp, Warning, TEXT("HitComponent: %s"), *HitComponent->GetName());
	// UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *OtherActor->GetName());
	// UE_LOG(LogTemp, Warning, TEXT("OtherComp: %s"), *OtherComp->GetName());
	//
	// // print other and other's velocity
	UE_LOG(LogTemp, Warning, TEXT("Self Velocity: %s"), *PhysicsMesh->GetPhysicsLinearVelocity().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Other Velocity: %s"), *OtherComp->GetPhysicsLinearVelocity().ToString());
	
	// only deal damage if LastVelocity moves self closer to other
	// aka they are in the same half space
	bool bDealDamage = false;
	if (not LastVelocity.IsNearlyZero())
	{
		FVector SelfToOther = OtherMarble->GetActorLocation() - GetActorLocation();
		float Projection = FVector::DotProduct(LastVelocity, SelfToOther);
		if (Projection > 0)
			bDealDamage = true;
	}

	if (not bDealDamage)
		return;
	
	if (CollisionParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionParticle, Hit.ImpactPoint);
	}

	FVector TargetDirn = -Hit.ImpactNormal;
	float Damage = CalculateDamage(LastVelocity, TargetDirn);
	UE_LOG(LogTemp, Warning, TEXT("%s tries to deal %f Damage to %s"), *GetName(), Damage, *OtherMarble->GetName());
	OtherMarble->CombatComponent->TakeDamage(Damage, this);
}

float ABaseMarble::CalculateDamage(FVector Velocity, FVector TargetDirn)
{
	// get velocity along the direction of the target
	float ContributingSpeed = FVector::DotProduct(Velocity, TargetDirn);
	float Damage = 0.5f * FMath::Pow((ContributingSpeed / 100.0), 2.0);
	return Damage;
}

void ABaseMarble::Die()
{
	bDead = true;
	UE_LOG(LogTemp, Warning, TEXT("%s has died"), *GetName());
	// float up to simulate a ghost
	PhysicsMesh->SetSimulatePhysics(false);
	PhysicsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PhysicsMesh->SetVisibility(false);
	StatusLabel->SetVisibility(false);
	HealthBar->SetVisibility(false);
	OutlineMesh->SetVisibility(false);
	
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += 10.0f * Radius;
	SetActorLocation(NewLocation);
}
