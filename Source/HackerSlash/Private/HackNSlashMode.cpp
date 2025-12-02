// Fill out your copyright notice in the Description page of Project Settings.


#include "HackNSlashMode.h"

AHackNSlashMode::AHackNSlashMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Game/Blueprints/Player/MainChar.MainChar_C"));
	DefaultPawnClass = PlayerPawnClass.Class;
}

void AHackNSlashMode::BeginPlay()
{
	Super::BeginPlay();
	
}
