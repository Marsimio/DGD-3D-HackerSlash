// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Player.h"

#include "DebuffComponent.h"
#include "Enemy.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "HealthUI.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PerkComponent.h"
#include "PerkDataAsset.h"
#include "StatsComponent.h"
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
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComp"));
	PerkComp  = CreateDefaultSubobject<UPerkComponent>(TEXT("PerkComp"));
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
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (HealthWidgetClass && HealthComp)
		{
			HealthWidgetInstance = CreateWidget<UHealthUI>(PlayerController, HealthWidgetClass);
			if (HealthWidgetInstance)
			{
				HealthWidgetInstance->AddToViewport();
				HealthWidgetInstance->InitWithHealthComponent(HealthComp);
			}
		}
	}
}

void ABP_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsLunging)
	{
		const float Step = FMath::Min(DeltaTime, LungeTimeRemaining);
		LungeTimeRemaining -= Step;

		LungeElapsed += Step;

		const float Alpha = FMath::Clamp(LungeElapsed / LungeTotalDuration, 0.f, 1.f);
		const float EaseAlpha = Alpha * Alpha * (3.f - 2.f * Alpha);

		const float DesiredDistance = LungeTotalDistance * EaseAlpha;

		const float DeltaDistance = DesiredDistance - LungeDistanceApplied;
		LungeDistanceApplied = DesiredDistance;

		FHitResult Hit;
		AddActorWorldOffset(LungeDirection * DeltaDistance, true, &Hit);

		if (Hit.bBlockingHit || LungeTimeRemaining <= 0.f)
		{
			bIsLunging = false;
		}
	}

	if (bAttackActive)
	{
		AttackActiveTimeRemaining -= DeltaTime;
		ApplySlashDamageTick();

		if (AttackActiveTimeRemaining <= 0.f)
		{
			bAttackActive = false;
		}
	}
	
	if (bAttackActive && bDrawAttackDebug)
	{
		const float HitRange = SlashRange;
		const float Radius   = SlashRadius;

		const FVector Forward = LungeDirection.IsNearlyZero() ? GetActorForwardVector() : LungeDirection.GetSafeNormal();
		const FVector Origin  = GetActorLocation() + Forward * (HitRange * 0.5f);

		// Visualize the overlap sphere
		DrawDebugSphere(GetWorld(), Origin, Radius, 24, FColor::Red, false, 0.f);

		// Visualize the arc cone (purely visual)
		const float HalfAngleRad = FMath::DegreesToRadians(SlashArcDegrees * 0.5f);
		DrawDebugCone(GetWorld(), GetActorLocation(), Forward, HitRange, HalfAngleRad, HalfAngleRad, 16, FColor::Red, false, 0.f);
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
		if (InputActions.MenuAction)
		{
			EnhancedInputComponent->BindAction(InputActions.MenuAction, ETriggerEvent::Started, this, &ABP_Player::OwnedPerksPressed);
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

void ABP_Player::OwnedPerksPressed()
{
	OnOwnedPerksRequested.Broadcast();
}

void ABP_Player::ApplySlashDamageTick()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float Damage = (StatsComp ? StatsComp->GetStat(EGameStatType::Damage) : 1.f);

	const float HitRange = SlashRange;
	const float Radius   = SlashRadius;

	const FVector Forward = LungeDirection.IsNearlyZero()
		? GetActorForwardVector()
		: LungeDirection.GetSafeNormal();

	const FVector Origin = GetActorLocation() + Forward * (HitRange * 0.5f);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ArcSlashTick), false);
	Params.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (!bHit) return;

	const float CosHalfArc = FMath::Cos(FMath::DegreesToRadians(SlashArcDegrees * 0.5f));
	AController* InstigatorController = Controller;

	for (const FOverlapResult& O : Overlaps)
	{
		AActor* HitActor = O.GetActor();
		if (!HitActor) continue;
		if (HitThisSwing.Contains(HitActor)) continue;

		AEnemy* Enemy = Cast<AEnemy>(HitActor);
		if (!Enemy) continue;

		FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();
		ToEnemy.Z = 0.f;

		if (ToEnemy.Size() > HitRange) continue;
		if (FVector::DotProduct(Forward, ToEnemy.GetSafeNormal()) < CosHalfArc) continue;

		HitThisSwing.Add(HitActor);

		FVector KnockDir = ToEnemy;
		if (!KnockDir.Normalize())
		{
			KnockDir = Forward;
		}

		const float KnockStrength = 2800.f;
		const float KnockDuration = 0.25f;

		if (UDebuffComponent* Debuff = Enemy->FindComponentByClass<UDebuffComponent>())
		{
			Debuff->ApplyKnockback(KnockDir, KnockStrength, KnockDuration);
		}


		UGameplayStatics::ApplyDamage(
			Enemy,
			Damage,
			InstigatorController,
			this,
			UDamageType::StaticClass()
		);
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
	UWorld* World = GetWorld();
	if (!World) return;

	if (!bCanAttack) return;
	bCanAttack = false;

	const float Cooldown = (StatsComp ? StatsComp->GetStat(EGameStatType::AttackCooldown) : 1.f);
	World->GetTimerManager().SetTimer(AttackCooldownTimer, this, &ABP_Player::ResetAttack, Cooldown, false);

	const float MoveSpeed = (StatsComp ? StatsComp->GetStat(EGameStatType::MoveSpeed) : 600.f);

	FVector Forward = GetActorForwardVector();
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		FHitResult Hit;
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			FVector Aim = Hit.Location - GetActorLocation();
			Aim.Z = 0.f;
			if (!Aim.IsNearlyZero())
			{
				Forward = Aim.GetSafeNormal();
			}
		}
	}

	LungeTotalDistance = MoveSpeed * LungeDistanceMultiplier;
	LungeTotalDuration = FMath::Max(0.01f, LungeDuration);
	LungeElapsed = 0.f;
	LungeDistanceApplied = 0.f;

	bIsLunging = true;
	LungeTimeRemaining = LungeTotalDuration;
	LungeDirection = Forward;

	bAttackActive = true;
	AttackActiveTimeRemaining = FMath::Max(0.01f, AttackActiveTime);

	HitThisSwing.Reset();
}

void ABP_Player::ResetAttack()
{
	bCanAttack = true;
}