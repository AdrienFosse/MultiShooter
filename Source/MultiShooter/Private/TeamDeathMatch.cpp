// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamDeathMatch.h"
#include "BasePlayerState.h"
#include "GameFramework/GameState.h"

ATeamDeathMatch::ATeamDeathMatch(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NbTeams = 2;
	bDelayedStart = true;
}

void ATeamDeathMatch::PostLogin(APlayerController* NewPlayer)
{
	int32 TeamNb;

	ABasePlayerState* NewPlayerState = CastChecked<ABasePlayerState>(NewPlayer->PlayerState);
	TeamNb = SelectTeam(NewPlayerState);
	NewPlayerState->SetTeam(TeamNb);

	Super::PostLogin(NewPlayer);
}

int32 ATeamDeathMatch::SelectTeam(ABasePlayerState* PlayerState)
{
	TArray<int32> CurrentTeam;
	CurrentTeam.AddZeroed(NbTeams);

	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ABasePlayerState* TestPlayerState = Cast<ABasePlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != PlayerState)
		{
			int32 TestPlayerTeam = TestPlayerState->GetTeam();
			if (CurrentTeam.IsValidIndex(TestPlayerTeam))
			{
				CurrentTeam[TestPlayerTeam]++;
			}
		}
	}

	int32 LeastPopulatedTeam = CurrentTeam[0];
	for (int32 i = 1; i < CurrentTeam.Num(); i++)
	{
		if (LeastPopulatedTeam > CurrentTeam[i])
		{
			LeastPopulatedTeam = CurrentTeam[i];
		}
	}

	TArray<int32> SuitableTeams;
	for (int32 i = 0; i < CurrentTeam.Num(); i++)
	{
		if (CurrentTeam[i] == LeastPopulatedTeam)
		{
			SuitableTeams.Add(i);
		}
	}

	return SuitableTeams[FMath::RandHelper(SuitableTeams.Num())];
}