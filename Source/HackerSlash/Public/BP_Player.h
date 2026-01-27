// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BP_Player.generated.h"

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
};

UCLASS()
class HACKERSLASH_API ABP_Player : public APawn
{
	GENERATED_BODY()

public:
	ABP_Player();

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
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Model",  meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PlayerMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
    UFloatingPawnMovement* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHealthComponent> HealthComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float fAttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float fAttackRange = 2000.0f; // max trace distance
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float fAttackRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	bool bDrawAttackDebug = true;
public:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category="Spawning")
	AActor* SpawnEnemy(TSubclassOf<AEnemy> EnemyClass);
	
private: 
	void Attack();
	void LookAt(const FVector& Location) const;
	void Move(const FInputActionValue& Value);
	void RotateRing(float DeltaTime);
	UFUNCTION()
	void OnPlayerDied(AActor* DeadActor);

};
