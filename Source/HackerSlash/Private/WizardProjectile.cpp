// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWizardProjectile::AWizardProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(12.f);
	Collision->SetSimulatePhysics(false);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWizardProjectile::OnProjOverlap);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(Collision);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetSimulatePhysics(false);
	
	InitialLifeSpan = BulletLife;

}

void AWizardProjectile::InitVelocity(const FVector& Vel)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = Vel;
	}
}

void AWizardProjectile::SetProjectileSpeed(float InProjectileSpeed)
{
	ProjectileSpeed = InProjectileSpeed;

	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = ProjectileSpeed;
		ProjectileMovement->MaxSpeed     = ProjectileSpeed;

		const FVector Dir = ProjectileMovement->Velocity.GetSafeNormal();
		if (!Dir.IsNearlyZero())
		{
			ProjectileMovement->Velocity = Dir * ProjectileSpeed;
		}
	}
}

void AWizardProjectile::SetBulletLife(float InBulletLife)
{
	BulletLife = InBulletLife;
	SetLifeSpan(BulletLife);
}



void AWizardProjectile::OnProjOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									 bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor->IsA(AWizardProjectile::StaticClass()))
	{
		return;
	}
	
	if (DamagedActors.Contains(OtherActor))
	{
		return;
	}

	DamagedActors.Add(OtherActor);

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		Destroy();
	}
}
