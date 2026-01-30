// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffComponent.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Navigation/CrowdFollowingComponent.h"

UDebuffComponent::UDebuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDebuffComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UDebuffComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bInterruptAIWhileKnocked)
	{
		SetAIMovementEnabled(true);
	}

	Super::EndPlay(EndPlayReason);
}

void UDebuffComponent::ApplyKnockback(const FVector& Direction, float Strength, float Duration, float UpwardStrength)
{
	if (!OwnerCharacter) return;

	FVector Dir = Direction;
	Dir.Z = 0.f;
	if (!Dir.Normalize()) return;

	KnockbackTimeRemaining = FMath::Max(Duration, 0.f);

	// Stop AI/crowd steering from fighting the knockback
	if (bInterruptAIWhileKnocked)
	{
		SetAIMovementEnabled(false);
	}

	// Immediate push
	const FVector LaunchVel = Dir * Strength + FVector(0.f, 0.f, UpwardStrength);
	OwnerCharacter->LaunchCharacter(LaunchVel, true, true);
}

void UDebuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (KnockbackTimeRemaining <= 0.f) return;

	KnockbackTimeRemaining = FMath::Max(0.f, KnockbackTimeRemaining - DeltaTime);

	if (KnockbackTimeRemaining <= 0.f)
	{
		if (bInterruptAIWhileKnocked)
		{
			SetAIMovementEnabled(true);
		}
	}
}

void UDebuffComponent::SetAIMovementEnabled(bool bEnabled)
{
	if (!OwnerCharacter) return;

	AAIController* AI = Cast<AAIController>(OwnerCharacter->GetController());
	if (!AI) return;

	if (!bEnabled)
	{
		AI->StopMovement();

		if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))
		{
			Crowd->SuspendCrowdSteering(true);
		}
	}
	else
	{
		if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(AI->GetPathFollowingComponent()))
		{
			Crowd->SuspendCrowdSteering(false);
		}
	}
}