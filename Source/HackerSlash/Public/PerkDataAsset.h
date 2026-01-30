// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PerkDataAsset.generated.h"

UENUM(BlueprintType)
enum class EGameStatType : uint8
{
	MoveSpeed,
	Damage,
	AttackCooldown,
	MaxHealth
};

UENUM(BlueprintType)
enum class EModOp : uint8
{
	Add,
	Multiply
};

USTRUCT(BlueprintType)
struct FStatMod
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGameStatType Stat = EGameStatType::MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EModOp Op = EModOp::Add;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value = 0.f;
};

UCLASS()
class HACKERSLASH_API UPerkDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PerkId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FStatMod> Mods;
};
