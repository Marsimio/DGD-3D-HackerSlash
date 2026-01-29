// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WizardProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class HACKERSLASH_API AWizardProjectile : public AActor
{
	GENERATED_BODY()

public:
	AWizardProjectile();

	void InitVelocity(const FVector& Vel);
	void SetDamage(float InDamage) { Damage = InDamage; }
	void SetProjectileSpeed(float InProjectileSpeed);
	void SetBulletLife(float InBulletLife);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> DamagedActors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	float ProjectileSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	float BulletLife = 4.f;

	UFUNCTION()
	void OnProjOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					   bool bFromSweep, const FHitResult& SweepResult);
};