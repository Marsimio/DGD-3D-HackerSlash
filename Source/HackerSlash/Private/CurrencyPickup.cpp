// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrencyPickup.h"

#include "Currency.h"
#include "Components/SphereComponent.h"

class UCurrency;

ACurrencyPickup::ACurrencyPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(Root);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACurrencyPickup::OnOverlapBegin);
}

void ACurrencyPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (UCurrency* CurrencyComp = OtherActor->FindComponentByClass<UCurrency>())
	{
		CurrencyComp->AddCurrency(CurrencyValue);
		Destroy();
	}
}