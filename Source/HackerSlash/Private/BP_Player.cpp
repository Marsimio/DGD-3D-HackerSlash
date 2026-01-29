// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Player.h"

#include "Enemy.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ABP_Player::ABP_Player()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoxCollision"));
	SetRootComponent(CapsuleComponent);

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(CapsuleComponent);
	
	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CursorMesh"));
	CursorMesh->SetupAttachment(CapsuleComponent);
	
	SpawnRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnRing"));
	SpawnRing->SetupAttachment(CapsuleComponent);
	
	SpawnPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(SpawnRing);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
}

void ABP_Player::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// PlayerController->bShowMouseCursor = true;
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (InputMapping)
				{
					Subsystem->AddMappingContext(InputMapping, 0);
				}
			}
		}
	}
	
	if (HealthComp)
	{
		HealthComp->OnDied.AddDynamic(this, &ABP_Player::OnPlayerDied);
	}
}

void ABP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bDrawZonesDebug)
	{
		
		DrawEncircleRings();
	}
	
	if (SpawnRing)
	{
		RotateRing(DeltaTime);
	}
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		FHitResult HitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult))
		{
			LookAt(HitResult.Location);
		}
	}

}

void ABP_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InputActions.MovementAction)
		{
			EnhancedInputComponent->BindAction(InputActions.MovementAction, ETriggerEvent::Triggered, this, &ABP_Player::Move);
		}
		if (InputActions.AttackAction)
		{
			EnhancedInputComponent->BindAction(InputActions.AttackAction, ETriggerEvent::Started, this, &ABP_Player::Attack);
		}
	}
}

void ABP_Player::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		// const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, -25, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection,   MovementVector.X);
	}
}

void ABP_Player::LookAt(const FVector& Location) const
{
	FVector TargetLocation = Location;
	TargetLocation.Z = GetActorLocation().Z;
	
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	if (PlayerMesh)
	{
		PlayerMesh->SetWorldRotation(Rotation);
	}
	if (CursorMesh)
	{
		CursorMesh->SetWorldLocation(TargetLocation);
	}
}


void ABP_Player::RotateRing(float DeltaTime)
{
	FRotator Rotation = FRotator(0.f, RingRotationSpeed * DeltaTime, 0.f);
	SpawnRing->AddLocalRotation(Rotation);
}

void ABP_Player::OnPlayerDied(AActor* DeadActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Died"));

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
}

AActor* ABP_Player::SpawnEnemy(TSubclassOf<AEnemy> EnemyClass)
{
	if (!EnemyClass || !SpawnPoint) return nullptr;
	
	const FVector SpawnLocation = SpawnPoint->GetComponentLocation();
	const FRotator SpawnRotation = SpawnPoint->GetComponentRotation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	
	AActor* SpawnedEnemy = GetWorld()->SpawnActor<AActor>(
	EnemyClass,
	SpawnLocation,
	SpawnRotation,
	SpawnParams
	);
	
	return SpawnedEnemy;
}


void ABP_Player::Attack()
{
	if (!CursorMesh) return;

	const FVector vAttackOrigin = CursorMesh->GetComponentLocation();
	const float fRadius = fAttackRadius;

	TArray<FOverlapResult> arrOverlaps;

	FCollisionQueryParams qParams;
	qParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->OverlapMultiByChannel(
		arrOverlaps,
		vAttackOrigin,
		FQuat::Identity,
		ECC_Pawn, // Enemies are Characters (Pawn)
		FCollisionShape::MakeSphere(fRadius),
		qParams
	);

	if (bDrawAttackDebug)
	{
		DrawDebugSphere(GetWorld(), vAttackOrigin, fRadius, 24, FColor::Red, false, 0.25f);
	}

	if (!bHit) return;

	AController* pInstigatorController = Cast<AController>(Controller);

	for (const FOverlapResult& rOverlap : arrOverlaps)
	{
		AActor* pHitActor = rOverlap.GetActor();
		if (!pHitActor) continue;

		AEnemy* pEnemy = Cast<AEnemy>(pHitActor);
		if (!pEnemy) continue;

		UGameplayStatics::ApplyDamage(
			pEnemy,
			fAttackDamage,
			pInstigatorController,
			this,
			UDamageType::StaticClass()
		);

		UE_LOG(LogTemp, Warning, TEXT("Cursor attack hit %s for %f"), *pEnemy->GetName(), fAttackDamage);
	}
}

void ABP_Player::DrawEncircleRings() const
{
	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Center = GetActorLocation() + FVector(0,0,10.f);

	const float Engage = 80.f;
	const float R1 = FMath::Max(Engage * 6.f, 120.f);
	const float R2 = FMath::Max(Engage *15.f,   120.f);
	const float R3 = FMath::Max(Engage * 30.f, 120.f);

	DrawDebugSphere(World, Center, R1, 64, FColor::Green,  false, 0.f, 0, 2.f);
	DrawDebugSphere(World, Center, R2, 64, FColor::Yellow, false, 0.f, 0, 2.f);
	DrawDebugSphere(World, Center, R3, 64, FColor::Red,    false, 0.f, 0, 2.f);
}


