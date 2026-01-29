// Fill out your copyright notice in the Description page of Project Settings.


#include "Wizard.h"

#include "WizardProjectile.h"

class AWizardProjectile;

void AWizard::DoHitCheck()
{
	if (!ProjectileClass) return;
	if (!IsValid(TargetPawn)) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLoc =
		GetActorLocation()
		+ GetActorForwardVector() * MuzzleForwardOffset
		+ FVector(0.f, 0.f, MuzzleZOffset);

	const FVector AimAt = TargetPawn->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FRotator SpawnRot = (AimAt - SpawnLoc).Rotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	AWizardProjectile* Proj = World->SpawnActor<AWizardProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
	if (!Proj) return;

	Proj->SetDamage(Damage); // damage comes from Wizard
	Proj->SetProjectileSpeed(ProjectileSpeed); 
	Proj->SetBulletLife(ProjectileLifeSpan); 
	Proj->InitVelocity(SpawnRot.Vector() * ProjectileSpeed);
}
