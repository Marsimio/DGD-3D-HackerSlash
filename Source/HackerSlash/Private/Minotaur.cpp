// Fill out your copyright notice in the Description page of Project Settings.


#include "Minotaur.h"

#include "AIController.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class AAIController;

void AMinotaur::Tick(float DeltaTime)
{
	if (bIsCharging)
	{
		UpdateCharge(DeltaTime);
		return;
	}

	Super::Tick(DeltaTime);
}

void AMinotaur::BeginPlay()
{
	Super::BeginPlay();
	StartCharge();
}

void AMinotaur::StartCharge()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		if (!bChargeMoveCached)
		{
			bChargeMoveCached = true;
			PreChargeMaxWalkSpeed = MoveComp->MaxWalkSpeed;
			PreChargeBrakingFrictionFactor = MoveComp->BrakingFrictionFactor;
			PreChargeBrakingDecel = MoveComp->BrakingDecelerationWalking;
			PreChargeOrientToMovement = MoveComp->bOrientRotationToMovement;
		}
	}
	
	HitActorsThisCharge.Reset();
	
	if (!IsValid(TargetPawn))
	{
		TargetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	if (!IsValid(TargetPawn)) return;

	bIsCharging = true;
	bHasChargeHit = false;
	ChargeTimer = ChargeDuration;

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}

	AimAtTarget();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->MaxWalkSpeed = ChargeSpeed;
		MoveComp->BrakingFrictionFactor = 0.f;
		MoveComp->BrakingDecelerationWalking = 0.f;

		MoveComp->AddInputVector(GetActorForwardVector(), true);
	}

	LaunchCharacter(GetActorForwardVector() * (ChargeSpeed * 0.6f), true, false);
}

void AMinotaur::UpdateCharge(float DeltaTime)
{
	ChargeTimer = FMath::Max(0.f, ChargeTimer - DeltaTime);

	if (IsValid(TargetPawn)) // Rotate towards player mid charge
	{
		const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
		const float TargetYaw = ToTarget.Rotation().Yaw;

		FRotator Current = GetActorRotation();
		const float NewYaw = FMath::FixedTurn(Current.Yaw, TargetYaw, ChargeTurnSpeedDegPerSec * DeltaTime);

		SetActorRotation(FRotator(0.f, NewYaw, 0.f));
	}
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->AddInputVector(GetActorForwardVector(), true);
	}

	if (!bHasChargeHit)
	{
		DoChargeHitCheck();
	}

	if (ChargeTimer <= 0.f)
	{
		StopCharge();
	}
}

void AMinotaur::StopCharge()
{
	bIsCharging = false;

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();

		if (bChargeMoveCached)
		{
			MoveComp->MaxWalkSpeed = PreChargeMaxWalkSpeed;
			MoveComp->BrakingFrictionFactor = PreChargeBrakingFrictionFactor;
			MoveComp->BrakingDecelerationWalking = PreChargeBrakingDecel;
			MoveComp->bOrientRotationToMovement = PreChargeOrientToMovement;
		}
		else
		{
			// fallback defaults
			MoveComp->bOrientRotationToMovement = true;
			MoveComp->BrakingFrictionFactor = 2.f;
			MoveComp->BrakingDecelerationWalking = 2048.f;
		}
	}
}

void AMinotaur::DoChargeHitCheck()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Center =
		GetActorLocation()
		+ GetActorForwardVector() * ChargeBoxForwardOffset
		+ FVector(0.f, 0.f, ChargeBoxHalfExtent.Z/2);

	const FQuat Rot = GetActorQuat();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MinotaurCharge), false);
	Params.AddIgnoredActor(this);
	
	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		Center,
		Rot,
		ECC_Pawn,
		FCollisionShape::MakeBox(ChargeBoxHalfExtent),
		Params
	);

	if (bDebugChargeHitbox)
	{
		DrawDebugBox(World, Center, ChargeBoxHalfExtent, Rot,
			bHit ? FColor::Red : FColor::Green, false, 0.05f);
	}

	if (!bHit) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	bool bHitPlayerThisTick = false;

	for (const FOverlapResult& O : Overlaps)
	{
		AActor* Other = O.GetActor();
		if (!IsValid(Other) || Other == this) continue;

		if (HitActorsThisCharge.Contains(Other)) continue;

		UGameplayStatics::ApplyDamage(
			Other,
			ChargeDamage,
			GetController(),
			this,
			UDamageType::StaticClass()
		);

		HitActorsThisCharge.Add(Other);

		if (Other == PlayerPawn || Cast<AMinotaur>(Other))
		{
			bHitPlayerThisTick = true;
		}
	}

	if (bHitPlayerThisTick)
	{
		bHasChargeHit = true;
		ChargeTimer = FMath::Min(ChargeTimer, PostHitChargeTimeCap);
	}
}