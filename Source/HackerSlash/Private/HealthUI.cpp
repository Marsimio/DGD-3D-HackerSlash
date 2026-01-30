// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthUI.h"

#include "Currency.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HealthComponent.h"

void UHealthUI::InitWithHealthComponent(UHealthComponent* InHealthComp)
{
	if (!InHealthComp) return;

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.RemoveDynamic(this, &UHealthUI::HandleHealthChanged);
	}

	HealthComp = InHealthComp;
	HealthComp->OnHealthChanged.AddDynamic(this, &UHealthUI::HandleHealthChanged);

	Refresh();
}

void UHealthUI::NativeDestruct()
{
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.RemoveDynamic(this, &UHealthUI::HandleHealthChanged);
	}
	if (CurrencyComp)
	{
		CurrencyComp->OnCurrencyChanged.RemoveDynamic(this, &UHealthUI::HandleCurrencyChanged);
	}
	Super::NativeDestruct();
}

void UHealthUI::InitWithCurrencyComponent(UCurrency* InCurrencyComp)
{
	CurrencyComp = InCurrencyComp;

	if (!CurrencyComp) return;

	CurrencyComp->OnCurrencyChanged.AddDynamic(this, &UHealthUI::HandleCurrencyChanged);

	HandleCurrencyChanged(CurrencyComp->GetCurrency());
}

void UHealthUI::HandleCurrencyChanged(int32 NewCurrency)
{

	if (Currency)
	{
		Currency->SetText(FText::AsNumber(NewCurrency));
	}
}

void UHealthUI::HandleHealthChanged(UHealthComponent* Comp, float NewHealth, float Delta, AActor* DamageCauser)
{
	Refresh();
}

void UHealthUI::Refresh()
{
	if (!PB_Health || !HealthComp) return;

	const float Max = FMath::Max(1.f, HealthComp->MaxHealth);
	const float Pct = FMath::Clamp(HealthComp->Health / Max, 0.f, 1.f);

	PB_Health->SetPercent(Pct);

	if (TXT_Health)
	{
		const int32 H = FMath::RoundToInt(HealthComp->Health);
		const int32 M = FMath::RoundToInt(Max);
		TXT_Health->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), H, M)));
	}
}