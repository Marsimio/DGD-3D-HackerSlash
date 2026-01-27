// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UHealthComponent;
class APawn;

UCLASS()
class HACKERSLASH_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
		
	UPROPERTY()
	TObjectPtr<APawn> TargetPawn; // targeting
	UFUNCTION()
	void OnEnemyDied(AActor* DeadActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHealthComponent> HealthComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AcceptanceRadius = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float RepathInterval = 0.25f;

	float RepathTimer = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float Damage = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackRange = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackCooldown = 1.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float HitWindowDuration = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	bool bDrawSlashDebug = true;

	bool bIsAttacking = false;
	bool bHasHitThisAttack = false;
	float CooldownTimer = 0.f;
	float HitWindowTimer = 0.f;

	virtual void TryStartAttack();
	virtual void StartAttack();
	virtual void UpdateAttack(float DeltaTime);

	virtual void FaceTarget();
	virtual void DoHitCheck();
	
	void BeginAttackCommon(float InCooldown, float InHitWindow);
	void EndAttackCommon();
private:
	void UpdateChase(float DeltaTime);

};
