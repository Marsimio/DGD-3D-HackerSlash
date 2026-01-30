// Fill out your copyright notice in the Description page of Project Settings.


#include "Currency.h"

UCurrency::UCurrency()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCurrency::AddCurrency(int32 Amount)
{
	if (Amount <= 0) return;
	Currency += Amount;
	UE_LOG(LogTemp, Warning, TEXT("[Currency] AddCurrency(%d) -> New Currency: %d"), Amount, Currency);
	
	OnCurrencyChanged.Broadcast(Currency);
}

void UCurrency::SetCurrency(int32 NewValue)
{
	Currency = FMath::Max(0, NewValue);
	OnCurrencyChanged.Broadcast(Currency); 
}

