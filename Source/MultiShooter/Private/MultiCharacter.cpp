#include "MultiCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseWeapon.h"
#include "HealthComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Engine/Engine.h"

AMultiCharacter::AMultiCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-40.0f, 1.75f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->bOnlyOwnerSee = true;
	FirstPersonMesh->bOwnerNoSee = false;
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FirstPersonMesh->SetRelativeLocation(FVector(-5.0f, -10.0f, -155.7f));

	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ReplicatedMesh"));
	ThirdPersonMesh->bOnlyOwnerSee = false;
	ThirdPersonMesh->bOwnerNoSee = true;
	ThirdPersonMesh->SetupAttachment(GetCapsuleComponent());
	ThirdPersonMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	ThirdPersonMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	ZoomFOV = 60.0f;
	ZoomSpeed = 20.0f;
}

void AMultiCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Controller && Controller->IsLocalPlayerController())
	{
		FirstPersonMesh->SetOwnerNoSee(false);
		ThirdPersonMesh->SetOwnerNoSee(true);
	}
	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AMultiCharacter::SpawnDefaultWeapon);
	}
}

void AMultiCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FirstPersonCamera->FieldOfView;
	TargetFOV = DefaultFOV;
	HealthComp->OnHealthChanged.AddDynamic(this, &AMultiCharacter::OnHealthChanged);
}

void AMultiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentFOV = FirstPersonCamera->FieldOfView;
	if (TargetFOV != CurrentFOV)
	{
		float NewFOV = FMath::FInterpTo(FirstPersonCamera->FieldOfView, TargetFOV, DeltaTime, ZoomSpeed);
		FirstPersonCamera->SetFieldOfView(NewFOV);
	}
}

void AMultiCharacter::SpawnDefaultWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("On Spawn Default Inventory"));
	if (GetLocalRole() < ROLE_Authority)
		return;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABaseWeapon* NewWeapon = (ABaseWeapon*)GetWorld()->SpawnActor<AActor>(DefaultWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	AddWeapon(NewWeapon);
	EquipWeapon(NewWeapon);
}

void AMultiCharacter::AddWeapon(ABaseWeapon* Weapon)
{
	if (GetLocalRole() < ROLE_Authority)
		return;
	Weapon->SetNewOwner(this);
	Inventory.AddUnique(Weapon);
}

void AMultiCharacter::RemoveWeapon(ABaseWeapon* Weapon)
{
	if (GetLocalRole() < ROLE_Authority)
		return;
	
}

void AMultiCharacter::EquipWeapon(ABaseWeapon* Weapon)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("On Equip Weapon"));
	if (Weapon)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerEquipWeapon(Weapon);
		}
		else
		{
			CurrentWeapon = Weapon;
			Weapon->SetNewOwner(this);
			Weapon->OnEquip();
		}
	}
}

void AMultiCharacter::ServerEquipWeapon_Implementation(ABaseWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool AMultiCharacter::ServerEquipWeapon_Validate(ABaseWeapon* Weapon)
{
	return true;
}

void AMultiCharacter::OnHealthChanged(UHealthComponent* HealthCompponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InvestigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Player is Dead!"));
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
	}
}

void AMultiCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMultiCharacter, Inventory, COND_OwnerOnly);

	DOREPLIFETIME(AMultiCharacter, CurrentWeapon);
}

void AMultiCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AMultiCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AMultiCharacter::StartReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void AMultiCharacter::StartZoom()
{
	TargetFOV = ZoomFOV;

}

void AMultiCharacter::EndZoom()
{
	TargetFOV = DefaultFOV;
}

void AMultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMultiCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AMultiCharacter::TurnAtRate);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMultiCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMultiCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultiCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMultiCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMultiCharacter::StartReload);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMultiCharacter::StartZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMultiCharacter::EndZoom);
}

void AMultiCharacter::MoveForward(float Value)
{
	if (Value != 0)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMultiCharacter::MoveRight(float Value)
{
	if (Value != 0)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMultiCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiCharacter::StartCrouch()
{
	Crouch();
}

void AMultiCharacter::EndCrouch()
{
	UnCrouch();
}

FVector AMultiCharacter::GetPawnViewLocation() const
{
	if (FirstPersonCamera)
	{
		return FirstPersonCamera->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}
