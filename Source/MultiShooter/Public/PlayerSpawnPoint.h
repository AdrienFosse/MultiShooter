// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerSpawnPoint.generated.h"

/**
 * Need to handle player death & respawn features before anythings else
 */
UCLASS()
class MULTISHOOTER_API APlayerSpawnPoint : public APlayerStart
{
	GENERATED_BODY()

	APlayerSpawnPoint(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditInstanceOnly, Category = Team)
	int32 SpawnTeamAllowed;
};
