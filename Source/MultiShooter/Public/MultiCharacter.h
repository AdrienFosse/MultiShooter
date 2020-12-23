	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class UInputComponent;
class ABaseWeapon;
class UHealthComponent;

UCLASS()
class MULTISHOOTER_API AMultiCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMultiCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void LookUpAtRate(float Rate);
	void TurnAtRate(float Rate);

	void StartCrouch();
	void EndCrouch();

	void StartFire();
	void StopFire();

	void StartReload();

	void StartZoom();
	void EndZoom();

	void EquipWeapon(ABaseWeapon* Weapon);

	/* Server Side Function */
	void SpawnDefaultWeapon();

	UFUNCTION(server, reliable, WithValidation)
	void ServerEquipWeapon(ABaseWeapon* Weapon);

	void AddWeapon(ABaseWeapon* Weapon);
	void RemoveWeapon(ABaseWeapon* Weapon);

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InvestigatedBy, AActor* DamageCauser);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	FORCEINLINE USkeletalMeshComponent* GetThirdPersonMesh() const { return ThirdPersonMesh; }
	FORCEINLINE FName GetWeaponAttachPoint() const { return WeaponSocketName;  }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* ThirdPersonMesh;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	UHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomSpeed;

	float DefaultFOV;

	float TargetFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ABaseWeapon> DefaultWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	FName WeaponSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

	UPROPERTY(Replicated)
	ABaseWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	TArray<class ABaseWeapon*> Inventory;


};
