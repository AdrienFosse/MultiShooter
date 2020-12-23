// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UAnimMontage;
class AMultiCharacter;

/*Weapon not use for the moment*/
namespace EWeaponState
{
	enum Type
	{
		Idle,
		OnFire,
		OnReload,
		OnEquip,
	};
}

USTRUCT()
struct FWeaponAnimation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FirstPerson;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ThirdPerson;
};

USTRUCT()
struct FHitScanData
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
	FVector_NetQuantize TraceFrom;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class MULTISHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

protected:


	virtual void BeginPlay() override;
	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector TraceFrom);
	void PlayFireEffects();
	void Fire();
	void Reload();
	void StopReload();
	float PlayAnimation(FWeaponAnimation AnimStruct);

	void AttachWeapon();

	USkeletalMeshComponent* GetWeaponMesh();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopReload();

	UFUNCTION()
	void OnRep_HitScan();

	UFUNCTION()
	void OnRep_CurrentOwner();

public:	
	
	void StartFire();
	void StopFire();
	void StartReload();
	void OnEquip();
	void SetNewOwner(AMultiCharacter* NewOwner);

protected:

	UPROPERTY(VisibleDefaultsOnly, category = Mesh)
	USkeletalMeshComponent* WeaponMesh1P;

	UPROPERTY(VisibleDefaultsOnly, category = Mesh)
	USkeletalMeshComponent* WeaponMesh3P;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* CharacterImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* HeadshotImpactEffect;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate;

	float TimeBetweenShots;

	float LastFireTime;

	FTimerHandle TimerHandle_TimeBetweenShots;
	FTimerHandle TimerHandle_Reload;
	FTimerHandle TimerHandle_StopReload;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 AmmoPerMag;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxAmmo;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmoMag;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, blueprintReadonly, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(ReplicatedUsing=OnRep_HitScan)
	FHitScanData HitScanNetData;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentOwner)
	AMultiCharacter* CurrentOwner;

	EWeaponState::Type CurrentState;
	
	/* Weapon Animations (Not totally implemented)*/

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FWeaponAnimation ReloadAnim;

};