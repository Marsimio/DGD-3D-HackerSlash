// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Navigation/CrowdFollowingComponent.h"


AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Crowd->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
		Crowd->SetCrowdSeparation(true);

		// Stable for big crowds (10 is often too aggressive)
		Crowd->SetCrowdSeparationWeight(4.0f);

		// 1000 is usually excessive and can make overreactions
		Crowd->SetCrowdCollisionQueryRange(200.0f);

		Crowd->SetCrowdAnticipateTurns(true);
	}
}