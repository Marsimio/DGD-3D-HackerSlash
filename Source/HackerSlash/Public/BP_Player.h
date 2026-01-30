// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BP_Player.generated.h"

class UCurrency;
class UHealthUI;
class UPerkComponent;
class UStatsComponent;
class UCapsuleComponent;
class AActor;
class AEnemy;
class UBoxComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;
class UInputMappingContext;
class UInputAction;
class UHealthComponent;

USTRUCT(BlueprintType)
struct FInputActions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MovementAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* AttackAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MenuAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* HealAction = nullptr;
};

DECLARE_MULTICAST_DELEGATE(FOnOwnedPerksRequested);

UCLASS()
class HACKERSLASH_API ABP_Player : public APawn
{
	GENERATED_BODY()

public:
	ABP_Player();
	
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UHealthUI> HealthWidgetClass;

	UPROPERTY()
	UHealthUI* HealthWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	FInputActions InputActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cursor")
	UStaticMeshComponent* CursorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	UStaticMeshComponent* SpawnRing;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	float RingRotationSpeed = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	UStaticMeshComponent* SpawnPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Model",  meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PlayerMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
    UFloatingPawnMovement* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHealthComponent> HealthComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStatsComponent> StatsComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPerkComponent> PerkComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCurrency> CurrencyComp;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Slash")
	float SlashArcDegrees = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Slash")
	float SlashRange = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Slash")
	float SlashRadius = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Lunge")
	float LungeSpeed = 1800.f;

	FVector LungeVelocity = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Lunge")
	float LungeDuration = 0.28f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bDrawAttackDebug = true;

	bool bIsLunging = false;
	float LungeTimeRemaining = 0.f;
	FVector LungeDirection = FVector::ZeroVector;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category="Spawning")
	AActor* SpawnEnemy(TSubclassOf<AEnemy> EnemyClass);
	
	FOnOwnedPerksRequested OnOwnedPerksRequested;
	
private: 
	bool bCanAttack = true;

	FTimerHandle AttackCooldownTimer;

	void ResetAttack();
	
	void Attack();
	void LookAt(const FVector& Location) const;
	void Move(const FInputActionValue& Value);
	void RotateRing(float DeltaTime);
	UFUNCTION()
	void OnPlayerDied(AActor* DeadActor);
	UFUNCTION()
	void OwnedPerksPressed();
	void HealPressed();

	// Lunge/attack window
	bool bAttackActive = false;
	float AttackActiveTimeRemaining = 0.f;

	float LungeTotalDuration = 0.f;
	float LungeElapsed = 0.f;
	float LungeTotalDistance = 0.f;
	float LungeDistanceApplied = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Lunge", meta=(AllowPrivateAccess="true"))
	float LungeDistanceMultiplier = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Slash", meta=(AllowPrivateAccess="true"))
	float AttackActiveTime = 0.12f;

	TSet<TWeakObjectPtr<AActor>> HitThisSwing;

	void ApplySlashDamageTick();
};