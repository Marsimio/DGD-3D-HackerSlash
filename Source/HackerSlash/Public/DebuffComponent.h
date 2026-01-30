// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebuffComponent.generated.h"

class ACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HACKERSLASH_API UDebuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDebuffComponent();

	UFUNCTION(BlueprintCallable, Category="Debuffs|Knockback")
	void ApplyKnockback(const FVector& Direction, float Strength, float Duration, float UpwardStrength = 0.f);

	UFUNCTION(BlueprintCallable, Category="Debuffs|Knockback")
	bool IsKnockedBack() const { return KnockbackTimeRemaining > 0.f; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category="Debuffs|Knockback")
	bool bInterruptAIWhileKnocked = true;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;

	float KnockbackTimeRemaining = 0.f;

	void SetAIMovementEnabled(bool bEnabled);
};