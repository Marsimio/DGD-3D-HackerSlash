// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

enum class EGameStatType : uint8;
class UPerkDataAsset;

USTRUCT(BlueprintType)
struct FStatBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float Damage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float AttackCooldown = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MaxHealth = 100.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsChanged);

USTRUCT(BlueprintType)
struct FAppliedPerk
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UPerkDataAsset> Perk = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PerkId;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKERSLASH_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOnStatsChanged OnStatsChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	FStatBlock BaseStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	FStatBlock FinalStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats|AutoApply")
	bool bAutoApplyMoveSpeed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats|AutoApply")
	bool bApplyToCharacterMovement = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats|AutoApply")
	bool bApplyToFloatingPawnMovement = true;

	UFUNCTION(BlueprintCallable, Category="Stats")
	float GetStat(EGameStatType Stat) const;

	UFUNCTION(BlueprintCallable, Category="Stats")
	const FStatBlock& GetFinalStats() const { return FinalStats; }

	UFUNCTION(BlueprintCallable, Category="Stats|Perks")
	bool AddPerk(const UPerkDataAsset* Perk);

	UFUNCTION(BlueprintCallable, Category="Stats|Perks")
	bool RemovePerkById(FName PerkId);

	UFUNCTION(BlueprintCallable, Category="Stats|Perks")
	bool HasPerk(FName PerkId) const;

	UFUNCTION(BlueprintCallable, Category="Stats|Perks")
	void ClearPerks();

	UFUNCTION(BlueprintCallable, Category="Stats")
	void RebuildStats();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Stats|Perks")
	TArray<FAppliedPerk> AppliedPerks;

	static float& StatRef(FStatBlock& Block, EGameStatType Stat);
	static float  StatVal(const FStatBlock& Block, EGameStatType Stat);

	void ApplyMoveSpeedToMovement();
};