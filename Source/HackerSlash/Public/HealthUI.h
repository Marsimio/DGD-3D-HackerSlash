// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthUI.generated.h"

class UCurrency;
class UProgressBar;
class UTextBlock;
class UHealthComponent;

UCLASS()
class HACKERSLASH_API UHealthUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWithHealthComponent(UHealthComponent* InHealthComp);
	
	UFUNCTION(BlueprintCallable)
	void InitWithCurrencyComponent(UCurrency* InCurrencyComp);

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* PB_Health = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* TXT_Health = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Currency;


	UFUNCTION()
	void HandleCurrencyChanged(int32 NewCurrency);
	
private:
	UPROPERTY()
	TObjectPtr<UHealthComponent> HealthComp = nullptr;
	UPROPERTY()
	TObjectPtr<UCurrency> CurrencyComp;

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* Comp, float NewHealth, float Delta, AActor* DamageCauser);

	void Refresh();
};