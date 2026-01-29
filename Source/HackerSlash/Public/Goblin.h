// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Goblin.generated.h"

/**
 * 
 */
UCLASS()
class HACKERSLASH_API AGoblin : public AEnemy
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	FVector BoxHalfExtent = FVector(60.f, 60.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	float BoxForwardOffset = 110.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	bool bDebugAttackBox = true;
	
protected:
	virtual void DoHitCheck() override;

private:
	void DoAttackHitbox();
};
