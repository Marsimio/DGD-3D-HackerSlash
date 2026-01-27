// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "AIController.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	bUseControllerRotationYaw = false;
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->bOrientRotationToMovement = true;
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnDied.AddDynamic(this, &AEnemy::OnEnemyDied);
	}

	TargetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CooldownTimer = FMath::Max(0.f, CooldownTimer - DeltaTime);

	if (bIsAttacking)
	{
		UpdateAttack(DeltaTime);
		return;
	}

	UpdateChase(DeltaTime);
	TryStartAttack();
}

void AEnemy::UpdateChase(float DeltaTime)
{
	if (!IsValid(TargetPawn)) return;

	AAIController* AI = Cast<AAIController>(GetController());
	if (!AI) return;

	RepathTimer += DeltaTime;
	if (RepathTimer < RepathInterval) return;
	RepathTimer = 0.0f;

	AI->MoveToActor(TargetPawn, AcceptanceRadius);
}


void AEnemy::TryStartAttack()
{
	if (!IsValid(TargetPawn)) return;
	if (CooldownTimer > 0.f) return;

	const float Dist2D = FVector::Dist2D(GetActorLocation(), TargetPawn->GetActorLocation());
	if (Dist2D > AttackRange) return;

	StartAttack();
}

void AEnemy::StartAttack()
{

}

void AEnemy::UpdateAttack(float DeltaTime)
{

}

void AEnemy::DoHitCheck()
{

}

void AEnemy::FaceTarget()
{
	if (!IsValid(TargetPawn)) return;

	const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
	const FRotator DesiredYaw(0.f, ToTarget.Rotation().Yaw, 0.f);

	SetActorRotation(DesiredYaw);
}

void AEnemy::BeginAttackCommon(float InCooldown, float InHitWindow)
{
	bIsAttacking = true;
	bHasHitThisAttack = false;
	CooldownTimer = InCooldown;
	HitWindowTimer = InHitWindow;

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}

	FaceTarget();
}

void AEnemy::EndAttackCommon()
{
	bIsAttacking = false;
}

void AEnemy::OnEnemyDied(AActor* DeadActor)
{
	Destroy();
}
