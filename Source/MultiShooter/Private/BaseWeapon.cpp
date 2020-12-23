// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "MultiCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	WeaponMesh1P->bReceivesDecals = false;
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = WeaponMesh1P;

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	WeaponMesh3P->bReceivesDecals = false;
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh3P->SetupAttachment(RootComponent);

	Damage = 20.0f;
	FireRate = 300.0f;

	AmmoPerMag = 30;
	MaxAmmo = 90;
	CurrentAmmoMag = AmmoPerMag;
	CurrentAmmo = MaxAmmo;

	SetReplicates(true);
	NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 42.0f;
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / FireRate;
}

void ABaseWeapon::OnEquip()
{
	AttachWeapon();
	if (CurrentOwner && CurrentOwner->IsLocallyControlled())
	{
		//Play Weapon Equip Sound
	}
}

void ABaseWeapon::AttachWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Attach Weapon On Owner: %s"), *CurrentOwner->GetName()));
	if (CurrentOwner)
	{
		FName WeaponAttachPoint = CurrentOwner->GetWeaponAttachPoint();
		// IsLocallyControlled always false, need to fix it
		//if (CurrentOwner->IsLocallyControlled())
		//{
			USkeletalMeshComponent* OwnerMesh1P = CurrentOwner->GetFirstPersonMesh();
			USkeletalMeshComponent* OwnerMesh3P = CurrentOwner->GetThirdPersonMesh();
			WeaponMesh1P->AttachToComponent(OwnerMesh1P, FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachPoint);
			WeaponMesh3P->AttachToComponent(OwnerMesh3P, FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachPoint);
		//}
		//else
		//{
		//	WeaponMesh3P->AttachToComponent(CurrentOwner->GetThirdPersonMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachPoint);
		//}
	}
}

USkeletalMeshComponent* ABaseWeapon::GetWeaponMesh()
{
	if (CurrentOwner && CurrentOwner->IsLocallyControlled())
	{
		return WeaponMesh1P;
	}
	return WeaponMesh3P;
}

void ABaseWeapon::StartFire()
{
	if (CurrentAmmoMag > 0)
	{
		float FirstDelay = FMath::Max(LastFireTime - GetWorld()->TimeSeconds + TimeBetweenShots, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ABaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else
	{
		//Play No Ammo Sound
	}
}

void ABaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ABaseWeapon::Fire()
{
	//Need to handle Ammunition, still have infinite ammo
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector  EyeLocation;
		FRotator EyeRotation;
		
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_GameTraceChannel1, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, EyeRotation.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint, EyeLocation);
		}
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		PlayFireEffects();
		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanNetData.SurfaceType = SurfaceType;
			HitScanNetData.TraceFrom = EyeLocation;
			HitScanNetData.TraceTo = TraceEnd;
		}
		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void ABaseWeapon::OnRep_HitScan()
{
	PlayFireEffects();
	PlayImpactEffects(HitScanNetData.SurfaceType, HitScanNetData.TraceTo, HitScanNetData.TraceFrom);
}

void ABaseWeapon::OnRep_CurrentOwner()
{

}

void ABaseWeapon::PlayFireEffects()
{
	UE_LOG(LogTemp, Log, TEXT("On Play Fire Effects"));
	if (MuzzleEffect)
	{
		UE_LOG(LogTemp, Log, TEXT("On MuzzleEffect"));
		if (CurrentOwner && CurrentOwner->IsLocallyControlled())
		{
			UE_LOG(LogTemp, Log, TEXT("On Locally Controlled"));
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh1P, MuzzleSocketName);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("On Not Locally Controlled"));
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMesh3P, MuzzleSocketName);
		}
	}
}

void ABaseWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector TraceFrom)
{
	UParticleSystem* EffectToPlay = nullptr;
	switch (SurfaceType)
	{
	case SurfaceType1:
		EffectToPlay = CharacterImpactEffect;
		UE_LOG(LogTemp, Log, TEXT("CharacterTouched"));
		break;
	case SurfaceType2:
		EffectToPlay = HeadshotImpactEffect;
		UE_LOG(LogTemp, Log, TEXT("CharacterHeadTouched"));
		break;
	default:
		EffectToPlay = DefaultImpactEffect;
		UE_LOG(LogTemp, Log, TEXT("OtherSurfaceTouched"));
		break;
	};

	if (EffectToPlay)
	{
		FVector ShotDir = ImpactPoint - TraceFrom;
		ShotDir.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EffectToPlay, ImpactPoint, ShotDir.Rotation());
	}
}

void ABaseWeapon::SetNewOwner(AMultiCharacter* NewOwner)
{
	if (CurrentOwner != NewOwner)
	{
		SetInstigator(NewOwner);
		SetOwner(NewOwner);
		CurrentOwner = NewOwner;
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseWeapon, HitScanNetData, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseWeapon, CurrentAmmoMag, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseWeapon, CurrentAmmo, COND_SkipOwner);
}

void ABaseWeapon::StartReload()
{
	//Neec to handle reload sound & animation
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}
	float AnimDuration = PlayAnimation(ReloadAnim);
	if (AnimDuration <= 0.0f)
	{
		AnimDuration = 2.5f;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseWeapon::StopReload, AnimDuration, false);
	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &ABaseWeapon::Reload, FMath::Max(0.1f, AnimDuration - 0.1f), false);
	}
}

float ABaseWeapon::PlayAnimation(FWeaponAnimation AnimStruct)
{
	float DurationTime = 0.0f;
	if (CurrentOwner)
	{
		UAnimMontage* Anim = AnimStruct.FirstPerson;
		if (Anim)
		{
			DurationTime = CurrentOwner->PlayAnimMontage(Anim);
		}
	}
	return DurationTime;
}

void ABaseWeapon::Reload()
{
	if (CurrentAmmo > 0 && CurrentAmmoMag < AmmoPerMag)
	{
		CurrentAmmoMag = FMath::Clamp(CurrentAmmo, 0, 30);
		CurrentAmmo -= CurrentAmmoMag;
	}
}

void ABaseWeapon::StopReload()
{

}

/* Server Side Function */

void ABaseWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ABaseWeapon::ServerFire_Validate()
{
	return true;
}

void ABaseWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool ABaseWeapon::ServerStartReload_Validate()
{
	return true;
}

void ABaseWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

bool ABaseWeapon::ServerStopReload_Validate()
{
	return true;
}