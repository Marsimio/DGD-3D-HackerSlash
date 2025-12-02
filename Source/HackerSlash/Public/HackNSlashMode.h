// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HackNSlashMode.generated.h"

/**
 * 
 */
UCLASS()
class HACKERSLASH_API AHackNSlashMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHackNSlashMode();
	
protected:
	virtual void BeginPlay() override;
	
};
