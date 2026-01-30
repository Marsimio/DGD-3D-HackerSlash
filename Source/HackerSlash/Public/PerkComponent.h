// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PerkComponent.generated.h"


class UStatsComponent;
class UPerkDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HACKERSLASH_API UPerkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPerkComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Perks")
	TArray<TObjectPtr<UPerkDataAsset>> OwnedPerks;

	UFUNCTION(BlueprintCallable, Category="Perks")
	bool ApplyPerk(UPerkDataAsset* Perk);

	UFUNCTION(BlueprintCallable, Category="Perks")
	bool HasPerkId(FName PerkId) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStatsComponent> StatsComp;
};