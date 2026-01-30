// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CurrencyPickup.generated.h"

class USphereComponent;
class UCurrency;
UCLASS()
class HACKERSLASH_API ACurrencyPickup : public AActor
{
	GENERATED_BODY()
	
public:
	ACurrencyPickup();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	int32 CurrencyValue = 1;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};