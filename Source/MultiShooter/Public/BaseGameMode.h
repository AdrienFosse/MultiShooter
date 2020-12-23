// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTISHOOTER_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABaseGameMode(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void PreInitializeComponents() override;
};
