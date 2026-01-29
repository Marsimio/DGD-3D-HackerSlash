// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmManager.generated.h"

class AEnemy;
class APawn;

UCLASS()
class HACKERSLASH_API ASwarmManager : public AActor
{
	GENERATED_BODY()

public:
	ASwarmManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void RegisterEnemy(AEnemy* Enemy);
	void UnregisterEnemy(AEnemy* Enemy);

	FVector GetGoalForEnemy(const AEnemy* Enemy) const;

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Swarm")
	TObjectPtr<APawn> TargetPawn;

	// Inner ring radius near player (melee pressure)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	float BaseRadius = 180.f;

	// Escape ring distance from the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	float RingSpacing = 180.f;

	// Enemy seperation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	float DesiredArcSpacing = 140.f;

	// Encirclement Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	float SwirlDegPerSec = 120.f;

	// Small random-ish per-enemy angle jitter (deg) to avoid perfect symmetry
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	float AngleJitterDeg = 6.f;

	// Nav projection extents
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	FVector NavProjectExtent = FVector(200.f, 200.f, 300.f);

	// If true, ring direction alternates by enemy unique id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swarm")
	bool bAlternateSwirlDirection = true;

private:
	UPROPERTY()
	TArray<TObjectPtr<AEnemy>> Enemies;

	UPROPERTY()
	TMap<TObjectPtr<AEnemy>, int32> EnemyToSlot;

	float PhaseDeg = 0.f;

	void CompactInvalid();
	int32 GetOrAssignSlot(const AEnemy* Enemy) const;
	void ComputeRingForSlot(int32 Slot, float& OutRadius, float& OutAngleDeg) const;
};