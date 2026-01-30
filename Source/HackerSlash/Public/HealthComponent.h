// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


class UStatsComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, UHealthComponent*, Comp, float, NewHealth, float, Delta,
                                              AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, DeadActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKERSLASH_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleStatsChanged();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float Health = MaxHealth;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDied OnDied;

	UFUNCTION(BlueprintCallable, Category="Health")
	void Heal(float fAmount);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool IsDead() const { return Health <= 0.f; }

private:
	UPROPERTY()
	TObjectPtr<UStatsComponent> StatsComp = nullptr;

	UPROPERTY(EditAnywhere, Category="Health")
	bool bUseStatsMaxHealth = true;

	UPROPERTY(EditAnywhere, Category="Health")
	bool bKeepHealthPercentOnMaxHealthChange = true;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};