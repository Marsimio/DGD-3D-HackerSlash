// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Currency.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyChanged, int32, NewCurrency);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKERSLASH_API UCurrency : public UActorComponent
{
	GENERATED_BODY()

public:
	UCurrency();
	UPROPERTY(BlueprintAssignable)
	FOnCurrencyChanged OnCurrencyChanged;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintCallable)
	void AddCurrency(int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrency(int32 NewValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Currency = 0;
};