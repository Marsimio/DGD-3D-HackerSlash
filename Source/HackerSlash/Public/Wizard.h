// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Wizard.generated.h"

class AWizardProjectile;
class AActor;

UCLASS()
class HACKERSLASH_API AWizard : public AEnemy
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat|Ranged")
	TSubclassOf<AWizardProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Combat|Ranged")
	float MuzzleForwardOffset = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Combat|Ranged")
	float MuzzleZOffset = 60.f;

	// Projectile tuning
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Combat|Ranged")
	float ProjectileSpeed = 1200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Combat|Ranged")
	float ProjectileLifeSpan = 1200.f;
	
	virtual void DoHitCheck() override;
};
