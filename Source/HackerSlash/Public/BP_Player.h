// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputActionValue.h"
#include "BP_Player.generated.h"

class UBoxComponent;

USTRUCT(BlueprintType)
struct FInputActions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MovementAction = nullptr;
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
	
	void Move(const FInputActionValue& Value);
	void LookAt(const FVector& Location) const;

protected:
	virtual void BeginPlay() override;

	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void RotateRing(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category="Spawning")
	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass);
};
