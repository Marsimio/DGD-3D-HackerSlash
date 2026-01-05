// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Player.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

ABP_Player::ABP_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(BoxCollision);
	
	CursorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CursorMesh"));
	CursorMesh->SetupAttachment(BoxCollision);
	
	SpawnRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnRing"));
	SpawnRing->SetupAttachment(BoxCollision);
	
	SpawnPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(SpawnRing);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
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
}

void ABP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
	}
}

void ABP_Player::Move(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Move called!"));

	FVector2D MovementVector = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Move vector: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);

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

AActor* ABP_Player::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
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


