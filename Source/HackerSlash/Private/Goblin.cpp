// Fill out your copyright notice in the Description page of Project Settings.


#include "Goblin.h"

#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

void AGoblin::DoHitCheck()
{
	DoAttackHitbox();
}

void AGoblin::DoAttackHitbox()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Center =
		GetActorLocation()
		+ GetActorForwardVector() * BoxForwardOffset
		+ FVector(0.f, 0.f, BoxHalfExtent.Z/2);

	const FQuat Rot = GetActorQuat();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GoblinAttackBox), false);
	Params.AddIgnoredActor(this);

	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		Center,
		Rot,
		ECC_Pawn,
		FCollisionShape::MakeBox(BoxHalfExtent),
		Params
	);

	if (bDebugAttackBox)
	{
		DrawDebugBox(World, Center, BoxHalfExtent, Rot, bHit ? FColor::Red : FColor::Green, false, 0.2f);
	}

	if (!bHit) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	for (const FOverlapResult& O : Overlaps)
	{
		AActor* Other = O.GetActor();
		if (!IsValid(Other) || Other == this) continue;

		if (Other != PlayerPawn) continue;

		UGameplayStatics::ApplyDamage(Other, Damage, GetController(), this, UDamageType::StaticClass());
		
		break;
	}
}
