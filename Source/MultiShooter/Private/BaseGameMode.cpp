// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"
#include "BasePlayerState.h"

ABaseGameMode::ABaseGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/BP_MultiCharacter"));
	DefaultPawnClass = PlayerPawn.Class;

	PlayerStateClass = ABasePlayerState::StaticClass();
}

void ABaseGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	StartMatch();
}