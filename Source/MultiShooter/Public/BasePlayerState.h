// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"


UCLASS()
class MULTISHOOTER_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void SetTeam(int32 Team);

	void SetKill(ABasePlayerState* Victim);

	void SetDeath(ABasePlayerState* KilledBy);

	int32 GetTeam();

	int32 GetKills();

	int32 GetDeaths();

protected:

	UPROPERTY()
	int32 CurrentTeam;

	UPROPERTY()
	int32 KillsNb;

	UPROPERTY()
	int32 DeathsNb;
};
