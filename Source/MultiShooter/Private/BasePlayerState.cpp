// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

void ABasePlayerState::SetTeam(int32 Team)
{

}

void ABasePlayerState::SetKill(ABasePlayerState* Victim)
{

}

void ABasePlayerState::SetDeath(ABasePlayerState* KilledBy)
{
}

int32 ABasePlayerState::GetTeam()
{
	return CurrentTeam;
}

int32 ABasePlayerState::GetKills()
{
	return KillsNb;
}

int32 ABasePlayerState::GetDeaths()
{
	return DeathsNb;
}