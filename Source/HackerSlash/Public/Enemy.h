// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UHealthComponent;
class APawn;
class ASwarmManager;
class UDebuffComponent;
class ACurrencyPickup;

UCLASS(Blueprintable, BlueprintType)
class HACKERSLASH_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<APawn> TargetPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHealthComponent> HealthComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UDebuffComponent> DebuffComp;
	
	UPROPERTY()
	TObjectPtr<ASwarmManager> SwarmMgr;

	UFUNCTION()
	void OnEnemyDied(AActor* DeadActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float Damage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackCooldown = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float HitWindowDuration = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") // range when enemy is considered "incombat" range
	float AttackRangeEnter = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat") // range when enemy gets taken out of combat range
	float AttackRangeExit = 230.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackLockDuration = 0.45f;

	float AttackLockTimer = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|AttackCommit") // range where the enemy ignores ring pathfinding and rushes in
	float AttackCommitDistance = 140.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|AttackCommit") // breaks off if they dont make it in time
	float AttackCommitDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|AttackCommit") // final how far in they want to be in the player
	float AttackCommitAcceptance = 40.f;
	
	UPROPERTY(EditAnywhere, Category="Drops")
	TSubclassOf<ACurrencyPickup> CurrencyPickupClass;

	UPROPERTY(EditAnywhere, Category="Drops")
	int32 CurrencyDropAmount = 1;

	virtual void TryStartAttack();
	virtual void StartAttack();
	void AimAtTarget();
	virtual void UpdateAttack(float DeltaTime);
	virtual void FaceTarget(float DeltaTime);
	virtual void DoHitCheck();

	void BeginAttackCommon(float InCooldown, float InHitWindow);
	void EndAttackCommon();

private:
	void UpdateSwarmMove(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|MoveTo", meta=(AllowPrivateAccess="true"))
	float RepathInterval = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|MoveTo", meta=(AllowPrivateAccess="true"))
	float AcceptanceRadius = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|MoveTo", meta=(AllowPrivateAccess="true"))
	float GoalUpdateMinDist = 50.f;

	float RepathTimer = 0.f;
	FVector LastGoal = FVector::ZeroVector;

	float OrbitSign = 1.f;

	float AttackCommitTimer = 0.f;
	
	// Attack state
	bool bIsAttacking = false;
	bool bHasFiredAttack = false;
	float CooldownTimer = 0.f;
	float HitWindowTimer = 0.f;
	bool bInCombatRange = false;

	// Cache movement during attack
	float CachedWalkSpeed = 0.f;
	float CachedAccel = 0.f;
	float CachedBraking = 0.f;
	bool  bCachedMoveValid = false;
};