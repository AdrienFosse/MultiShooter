// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedDelegate, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InvestigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup = (MULTICOMP), meta = (BlueprintSpawnableComponent))
class MULTISHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Health")
	float DefaultHealth;

	UPROPERTY(Replicated)
	float CurrentHealth;

public:

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHealthChangedDelegate OnHealthChanged;
};
