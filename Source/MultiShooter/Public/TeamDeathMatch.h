// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "TeamDeathMatch.generated.h"

class ABasePlayerState;

UCLASS()
class MULTISHOOTER_API ATeamDeathMatch : public ABaseGameMode
{
	GENERATED_BODY()

	ATeamDeathMatch(const FObjectInitializer& ObjectInitializer);

	void PostLogin(APlayerController* NewPlayer) override;

protected:

	int32 NbTeams;

	int32 WinnerTeam;

	int32 SelectTeam(ABasePlayerState* PlayerState);
};
