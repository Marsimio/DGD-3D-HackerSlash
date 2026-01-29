// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Goblin.h"
#include "Minotaur.generated.h"

/**
 * 
 */
UCLASS()
class HACKERSLASH_API AMinotaur : public AGoblin
{
	GENERATED_BODY()
	

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	float ChargeSpeed = 1200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	float ChargeTurnSpeedDegPerSec = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	float ChargeDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	float ChargeDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	FVector ChargeBoxHalfExtent = FVector(70.f, 70.f, 80.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	float ChargeBoxForwardOffset = 140.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Charge")
	bool bDebugChargeHitbox = true;
	
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisCharge;

private:
	bool bIsCharging = false;
	bool bHasChargeHit = false;
	float ChargeTimer = 0.f;

	void StartCharge();
	void UpdateCharge(float DeltaTime);
	void StopCharge();

	void DoChargeHitCheck();
};