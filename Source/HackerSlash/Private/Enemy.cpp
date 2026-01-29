// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemyAIController.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "NavigationSystem.h"
#include "SwarmManager.h"
#include "Engine/OverlapResult.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickInterval = 0.05f;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	OrbitSign = ((GetUniqueID() & 1) == 0) ? 1.f : -1.f;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bUseRVOAvoidance = false;
	}

	if (HealthComp)
	{
		HealthComp->OnDied.AddDynamic(this, &AEnemy::OnEnemyDied);
	}

	TargetPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	SwarmMgr = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
	
	if (SwarmMgr)
	{
		SwarmMgr->RegisterEnemy(this);
	}
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
	
	if (IsValid(TargetPawn))
	{
		const float Dist2D = FVector::Dist2D(GetActorLocation(), TargetPawn->GetActorLocation());
		if (Dist2D <= AttackRangeEnter) bInCombatRange = true;
		if (Dist2D >  AttackRangeExit)  bInCombatRange = false;
	}
	
	if (bInCombatRange)
	{
		TryStartAttack();
		FaceTarget(DeltaTime);
		return;
	}

	UpdateSwarmMove(DeltaTime);
	TryStartAttack();
}

void AEnemy::UpdateSwarmMove(float DeltaTime)
{
	if (!IsValid(TargetPawn) || !IsValid(SwarmMgr)) return;

	AAIController* AI = Cast<AAIController>(GetController());
	if (!AI) return;

	const FVector MyLoc = GetActorLocation();
	const FVector PlayerLoc = TargetPawn->GetActorLocation();
	const float Dist2D = FVector::Dist2D(MyLoc, PlayerLoc);

	if (CooldownTimer <= 0.f && Dist2D <= AttackRangeExit)
	{
		AttackCommitTimer = AttackCommitDuration;
	}

	AttackCommitTimer = FMath::Max(0.f, AttackCommitTimer - DeltaTime);

	if (AttackCommitTimer > 0.f)
	{
		AI->MoveToLocation(PlayerLoc, AttackCommitAcceptance, false);
		return;
	}
	
	RepathTimer += DeltaTime;
	if (RepathTimer < RepathInterval) return;
	RepathTimer = 0.f;

	FVector Goal = SwarmMgr->GetGoalForEnemy(this);

	if (!LastGoal.IsZero() && FVector::Dist2D(Goal, LastGoal) < GoalUpdateMinDist)
	{
		return;
	}

	LastGoal = Goal;
	AI->MoveToLocation(Goal, AcceptanceRadius, false);
}

void AEnemy::TryStartAttack()
{
	if (!IsValid(TargetPawn)) return;
	if (CooldownTimer > 0.f) return;

	const float Dist2D = FVector::Dist2D(GetActorLocation(), TargetPawn->GetActorLocation());

	if (!bInCombatRange)
	{
		if (Dist2D > AttackRangeEnter) return;
		bInCombatRange = true;
	}

	StartAttack();
}

void AEnemy::StartAttack()
{
	BeginAttackCommon(AttackCooldown, HitWindowDuration);
}

void AEnemy::AimAtTarget()
{
	if (IsValid(TargetPawn))
	{
		const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
		SetActorRotation(FRotator(0.f, ToTarget.Rotation().Yaw, 0.f));
	}
}

void AEnemy::BeginAttackCommon(float InCooldown, float InHitWindow)
{
	bIsAttacking = true;
	bHasFiredAttack = false;

	CooldownTimer = InCooldown;
	HitWindowTimer = InHitWindow;

	AttackLockTimer = AttackLockDuration;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		CachedWalkSpeed = Move->MaxWalkSpeed;
		CachedAccel     = Move->MaxAcceleration;
		CachedBraking   = Move->BrakingDecelerationWalking;
		bCachedMoveValid = true;

		Move->StopMovementImmediately();
		Move->MaxWalkSpeed = 0.f;
		Move->MaxAcceleration = 0.f;
		Move->BrakingDecelerationWalking = 999999.f;
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();

		if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))
		{
			Crowd->SuspendCrowdSteering(true);
		}
	}

	AimAtTarget();
}

void AEnemy::UpdateAttack(float DeltaTime)
{
	
	HitWindowTimer   = FMath::Max(0.f, HitWindowTimer - DeltaTime);
	AttackLockTimer  = FMath::Max(0.f, AttackLockTimer - DeltaTime);

	if (HitWindowTimer > 0.f && !bHasFiredAttack)
	{
		DoHitCheck();
		bHasFiredAttack = true;
	}

	if (AttackLockTimer <= 0.f)
	{
		EndAttackCommon();
	}
}

void AEnemy::EndAttackCommon()
{
	bIsAttacking = false;
	bHasFiredAttack = false;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		if (bCachedMoveValid)
		{
			Move->MaxWalkSpeed = CachedWalkSpeed;
			Move->MaxAcceleration = CachedAccel;
			Move->BrakingDecelerationWalking = CachedBraking;
			bCachedMoveValid = false;
		}
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))
		{
			Crowd->SuspendCrowdSteering(false);
		}
	}

	AttackCommitTimer = 0.f;
	
	if (!bInCombatRange)
	{
		LastGoal = FVector::ZeroVector;
		RepathTimer = RepathInterval;
	}
}

void AEnemy::DoHitCheck()
{
	// Subclasses implement (Wizard fires projectile, etc.)
}

void AEnemy::FaceTarget(float DeltaTime)
{
	if (!IsValid(TargetPawn)) return;

	const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
	const FRotator Desired(0.f, ToTarget.Rotation().Yaw, 0.f);

	const float InterpSpeed = 10.f;
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Desired, DeltaTime, InterpSpeed);

	SetActorRotation(NewRot);
}

void AEnemy::OnEnemyDied(AActor* DeadActor)
{
	if (SwarmMgr)
	{
		SwarmMgr->UnregisterEnemy(this);
	}
	Destroy();
}